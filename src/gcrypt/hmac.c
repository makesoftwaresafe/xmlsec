/*
 * XML Security Library (http://www.aleksey.com/xmlsec).
 *
 * HMAC transforms implementation for GCrypt.
 *
 * This is free software; see Copyright file in the source
 * distribution for preciese wording.
 *
 * Copyright (C) 2002-2024 Aleksey Sanin <aleksey@aleksey.com>. All Rights Reserved.
 */
/**
 * SECTION:crypto
 */

#ifndef XMLSEC_NO_HMAC
#include "globals.h"

#include <string.h>

#include <gcrypt.h>

#include <xmlsec/xmlsec.h>
#include <xmlsec/errors.h>
#include <xmlsec/keys.h>
#include <xmlsec/private.h>

#include <xmlsec/gcrypt/app.h>
#include <xmlsec/gcrypt/crypto.h>

#include "../cast_helpers.h"
#include "../keysdata_helpers.h"
#include "../transform_helpers.h"

/**************************************************************************
 *
 * Internal GCRYPT HMAC CTX
 *
 *****************************************************************************/
typedef struct _xmlSecGCryptHmacCtx             xmlSecGCryptHmacCtx, *xmlSecGCryptHmacCtxPtr;
struct _xmlSecGCryptHmacCtx {
    int                 digest;
    gcry_md_hd_t        digestCtx;
    xmlSecByte          dgst[XMLSEC_TRANSFORM_HMAC_MAX_OUTPUT_SIZE];
    xmlSecSize          dgstSizeInBits;       /* dgst size in bits */
};

/******************************************************************************
 *
 * HMAC transforms
 *
 * xmlSecTransform + xmlSecGCryptHmacCtx
 *
 *****************************************************************************/
XMLSEC_TRANSFORM_DECLARE(GCryptHmac, xmlSecGCryptHmacCtx)
#define xmlSecGCryptHmacSize XMLSEC_TRANSFORM_SIZE(GCryptHmac)

static int      xmlSecGCryptHmacCheckId                 (xmlSecTransformPtr transform);
static int      xmlSecGCryptHmacInitialize              (xmlSecTransformPtr transform);
static void     xmlSecGCryptHmacFinalize                (xmlSecTransformPtr transform);
static int      xmlSecGCryptHmacNodeRead                (xmlSecTransformPtr transform,
                                                         xmlNodePtr node,
                                                         xmlSecTransformCtxPtr transformCtx);
static int      xmlSecGCryptHmacSetKeyReq               (xmlSecTransformPtr transform,
                                                         xmlSecKeyReqPtr keyReq);
static int      xmlSecGCryptHmacSetKey                  (xmlSecTransformPtr transform,
                                                         xmlSecKeyPtr key);
static int      xmlSecGCryptHmacVerify                  (xmlSecTransformPtr transform,
                                                         const xmlSecByte* data,
                                                         xmlSecSize dataSize,
                                                         xmlSecTransformCtxPtr transformCtx);
static int      xmlSecGCryptHmacExecute                 (xmlSecTransformPtr transform,
                                                         int last,
                                                         xmlSecTransformCtxPtr transformCtx);

static int
xmlSecGCryptHmacCheckId(xmlSecTransformPtr transform) {

#ifndef XMLSEC_NO_SHA1
    if(xmlSecTransformCheckId(transform, xmlSecGCryptTransformHmacSha1Id)) {
        return(1);
    } else
#endif /* XMLSEC_NO_SHA1 */

#ifndef XMLSEC_NO_SHA256
    if(xmlSecTransformCheckId(transform, xmlSecGCryptTransformHmacSha256Id)) {
        return(1);
    } else
#endif /* XMLSEC_NO_SHA256 */

#ifndef XMLSEC_NO_SHA384
    if(xmlSecTransformCheckId(transform, xmlSecGCryptTransformHmacSha384Id)) {
        return(1);
    } else
#endif /* XMLSEC_NO_SHA384 */

#ifndef XMLSEC_NO_SHA512
    if(xmlSecTransformCheckId(transform, xmlSecGCryptTransformHmacSha512Id)) {
        return(1);
    } else
#endif /* XMLSEC_NO_SHA512 */

#ifndef XMLSEC_NO_RIPEMD160
    if(xmlSecTransformCheckId(transform, xmlSecGCryptTransformHmacRipemd160Id)) {
        return(1);
    } else
#endif /* XMLSEC_NO_RIPEMD160 */

#ifndef XMLSEC_NO_MD5
    if(xmlSecTransformCheckId(transform, xmlSecGCryptTransformHmacMd5Id)) {
        return(1);
    } else
#endif /* XMLSEC_NO_MD5 */

    /* not found */
    {
        return(0);
    }

    /* just in case */
    return(0);
}


static int
xmlSecGCryptHmacInitialize(xmlSecTransformPtr transform) {
    xmlSecGCryptHmacCtxPtr ctx;
    xmlSecSize hmacSize;
    gcry_error_t err;

    xmlSecAssert2(xmlSecGCryptHmacCheckId(transform), -1);
    xmlSecAssert2(xmlSecTransformCheckSize(transform, xmlSecGCryptHmacSize), -1);

    ctx = xmlSecGCryptHmacGetCtx(transform);
    xmlSecAssert2(ctx != NULL, -1);

    memset(ctx, 0, sizeof(xmlSecGCryptHmacCtx));

#ifndef XMLSEC_NO_SHA1
    if(xmlSecTransformCheckId(transform, xmlSecGCryptTransformHmacSha1Id)) {
        ctx->digest = GCRY_MD_SHA1;
    } else
#endif /* XMLSEC_NO_SHA1 */

#ifndef XMLSEC_NO_SHA256
    if(xmlSecTransformCheckId(transform, xmlSecGCryptTransformHmacSha256Id)) {
        ctx->digest = GCRY_MD_SHA256;
    } else
#endif /* XMLSEC_NO_SHA256 */

#ifndef XMLSEC_NO_SHA384
    if(xmlSecTransformCheckId(transform, xmlSecGCryptTransformHmacSha384Id)) {
        ctx->digest = GCRY_MD_SHA384;
    } else
#endif /* XMLSEC_NO_SHA384 */

#ifndef XMLSEC_NO_SHA512
    if(xmlSecTransformCheckId(transform, xmlSecGCryptTransformHmacSha512Id)) {
        ctx->digest = GCRY_MD_SHA512;
    } else
#endif /* XMLSEC_NO_SHA512 */

#ifndef XMLSEC_NO_RIPEMD160
    if(xmlSecTransformCheckId(transform, xmlSecGCryptTransformHmacRipemd160Id)) {
        ctx->digest = GCRY_MD_RMD160;
    } else
#endif /* XMLSEC_NO_RIPEMD160 */

#ifndef XMLSEC_NO_MD5
    if(xmlSecTransformCheckId(transform, xmlSecGCryptTransformHmacMd5Id)) {
        ctx->digest = GCRY_MD_MD5;
    } else
#endif /* XMLSEC_NO_MD5 */

    /* not found */
    {
        xmlSecInvalidTransfromError(transform)
        return(-1);
    }

    hmacSize = gcry_md_get_algo_dlen(ctx->digest);
    xmlSecAssert2(hmacSize > 0, -1);
    xmlSecAssert2(hmacSize <= XMLSEC_TRANSFORM_HMAC_MAX_OUTPUT_SIZE, -1);
    ctx->dgstSizeInBits = 8 * hmacSize;

    /* open context */
    err = gcry_md_open(&ctx->digestCtx, ctx->digest, GCRY_MD_FLAG_HMAC | GCRY_MD_FLAG_SECURE); /* we are paranoid */
    if(err != GPG_ERR_NO_ERROR) {
        xmlSecGCryptError("gcry_md_open", err,
                          xmlSecTransformGetName(transform));
        return(-1);
    }

    return(0);
}

static void
xmlSecGCryptHmacFinalize(xmlSecTransformPtr transform) {
    xmlSecGCryptHmacCtxPtr ctx;

    xmlSecAssert(xmlSecGCryptHmacCheckId(transform));
    xmlSecAssert(xmlSecTransformCheckSize(transform, xmlSecGCryptHmacSize));

    ctx = xmlSecGCryptHmacGetCtx(transform);
    xmlSecAssert(ctx != NULL);

    if(ctx->digestCtx != NULL) {
        gcry_md_close(ctx->digestCtx);
    }
    memset(ctx, 0, sizeof(xmlSecGCryptHmacCtx));
}

static int
xmlSecGCryptHmacNodeRead(xmlSecTransformPtr transform, xmlNodePtr node,
                         xmlSecTransformCtxPtr transformCtx XMLSEC_ATTRIBUTE_UNUSED) {
    xmlSecGCryptHmacCtxPtr ctx;
    int ret;

    xmlSecAssert2(xmlSecGCryptHmacCheckId(transform), -1);
    xmlSecAssert2(xmlSecTransformCheckSize(transform, xmlSecGCryptHmacSize), -1);
    xmlSecAssert2(node != NULL, -1);
    UNREFERENCED_PARAMETER(transformCtx);

    ctx = xmlSecGCryptHmacGetCtx(transform);
    xmlSecAssert2(ctx != NULL, -1);

    ret = xmlSecTransformHmacReadOutputBitsSize(node, ctx->dgstSizeInBits, &ctx->dgstSizeInBits);
    if (ret < 0) {
        xmlSecInternalError("xmlSecTransformHmacReadOutputBitsSize()",
            xmlSecTransformGetName(transform));
        return(-1);
    }
    xmlSecAssert2(ctx->dgstSizeInBits > 0, -1);
    xmlSecAssert2(XMLSEC_TRANSFORM_HMAC_BITS_TO_BYTES(ctx->dgstSizeInBits) < XMLSEC_TRANSFORM_HMAC_MAX_OUTPUT_SIZE, -1);

    return(0);
}


static int
xmlSecGCryptHmacSetKeyReq(xmlSecTransformPtr transform,  xmlSecKeyReqPtr keyReq) {
    xmlSecGCryptHmacCtxPtr ctx;

    xmlSecAssert2(xmlSecGCryptHmacCheckId(transform), -1);
    xmlSecAssert2((transform->operation == xmlSecTransformOperationSign) || (transform->operation == xmlSecTransformOperationVerify), -1);
    xmlSecAssert2(keyReq != NULL, -1);
    xmlSecAssert2(xmlSecTransformCheckSize(transform, xmlSecGCryptHmacSize), -1);

    ctx = xmlSecGCryptHmacGetCtx(transform);
    xmlSecAssert2(ctx != NULL, -1);

    keyReq->keyId  = xmlSecGCryptKeyDataHmacId;
    keyReq->keyType= xmlSecKeyDataTypeSymmetric;
    if(transform->operation == xmlSecTransformOperationSign) {
        keyReq->keyUsage = xmlSecKeyUsageSign;
    } else {
        keyReq->keyUsage = xmlSecKeyUsageVerify;
    }

    return(0);
}

static int
xmlSecGCryptHmacSetKey(xmlSecTransformPtr transform, xmlSecKeyPtr key) {
    xmlSecGCryptHmacCtxPtr ctx;
    xmlSecKeyDataPtr value;
    xmlSecBufferPtr buffer;
    gcry_error_t err;

    xmlSecAssert2(xmlSecGCryptHmacCheckId(transform), -1);
    xmlSecAssert2((transform->operation == xmlSecTransformOperationSign) || (transform->operation == xmlSecTransformOperationVerify), -1);
    xmlSecAssert2(xmlSecTransformCheckSize(transform, xmlSecGCryptHmacSize), -1);
    xmlSecAssert2(key != NULL, -1);

    ctx = xmlSecGCryptHmacGetCtx(transform);
    xmlSecAssert2(ctx != NULL, -1);
    xmlSecAssert2(ctx->digestCtx != NULL, -1);

    value = xmlSecKeyGetValue(key);
    xmlSecAssert2(xmlSecKeyDataCheckId(value, xmlSecGCryptKeyDataHmacId), -1);

    buffer = xmlSecKeyDataBinaryValueGetBuffer(value);
    xmlSecAssert2(buffer != NULL, -1);

    if(xmlSecBufferGetSize(buffer) == 0) {
        xmlSecInvalidZeroKeyDataSizeError(xmlSecTransformGetName(transform));
        return(-1);
    }

    err = gcry_md_setkey(ctx->digestCtx, xmlSecBufferGetData(buffer),
                        xmlSecBufferGetSize(buffer));
    if(err != GPG_ERR_NO_ERROR) {
        xmlSecGCryptError("gcry_md_setkey", err,
                          xmlSecTransformGetName(transform));
        return(-1);
    }
    return(0);
}

static int
xmlSecGCryptHmacVerify(xmlSecTransformPtr transform,
                        const xmlSecByte* data, xmlSecSize dataSize,
                        xmlSecTransformCtxPtr transformCtx XMLSEC_ATTRIBUTE_UNUSED) {
    xmlSecGCryptHmacCtxPtr ctx;
    int ret;

    xmlSecAssert2(xmlSecTransformIsValid(transform), -1);
    xmlSecAssert2(xmlSecTransformCheckSize(transform, xmlSecGCryptHmacSize), -1);
    xmlSecAssert2(transform->operation == xmlSecTransformOperationVerify, -1);
    xmlSecAssert2(transform->status == xmlSecTransformStatusFinished, -1);
    xmlSecAssert2(data != NULL, -1);
    UNREFERENCED_PARAMETER(transformCtx);

    ctx = xmlSecGCryptHmacGetCtx(transform);
    xmlSecAssert2(ctx != NULL, -1);
    xmlSecAssert2(ctx->digestCtx != NULL, -1);
    xmlSecAssert2(ctx->dgstSizeInBits > 0, -1);

    /* Returns 1 for match, 0 for no match, <0 for errors. */
    ret = xmlSecTransformHmacVerify(data, dataSize, ctx->dgst, ctx->dgstSizeInBits, sizeof(ctx->dgst));
    if(ret < 0) {
        xmlSecInternalError("xmlSecTransformHmacVerify", xmlSecTransformGetName(transform));
        return(-1);
    }
    if(ret == 1) {
        transform->status = xmlSecTransformStatusOk;
    } else {
        transform->status = xmlSecTransformStatusFail;
    }

    /* done */
    return(0);
}

static int
xmlSecGCryptHmacExecute(xmlSecTransformPtr transform, int last, xmlSecTransformCtxPtr transformCtx) {
    xmlSecGCryptHmacCtxPtr ctx;
    xmlSecBufferPtr in, out;
    xmlSecByte* dgst;
    int ret;

    xmlSecAssert2(xmlSecGCryptHmacCheckId(transform), -1);
    xmlSecAssert2((transform->operation == xmlSecTransformOperationSign) || (transform->operation == xmlSecTransformOperationVerify), -1);
    xmlSecAssert2(xmlSecTransformCheckSize(transform, xmlSecGCryptHmacSize), -1);
    xmlSecAssert2(transformCtx != NULL, -1);

    ctx = xmlSecGCryptHmacGetCtx(transform);
    xmlSecAssert2(ctx != NULL, -1);
    xmlSecAssert2(ctx->digestCtx != NULL, -1);

    in = &(transform->inBuf);
    out = &(transform->outBuf);

    if(transform->status == xmlSecTransformStatusNone) {
        transform->status = xmlSecTransformStatusWorking;
    }

    if(transform->status == xmlSecTransformStatusWorking) {
        xmlSecSize inSize;

        inSize = xmlSecBufferGetSize(in);
        if(inSize > 0) {
            gcry_md_write(ctx->digestCtx, xmlSecBufferGetData(in), inSize);

            ret = xmlSecBufferRemoveHead(in, inSize);
            if(ret < 0) {
                xmlSecInternalError2("xmlSecBufferRemoveHead",
                                     xmlSecTransformGetName(transform),
                                     "size=" XMLSEC_SIZE_FMT, inSize);
                return(-1);
            }
        }
        if(last) {
            /* get the final digest */
            gcry_md_final(ctx->digestCtx);
            dgst = gcry_md_read(ctx->digestCtx, ctx->digest);
            if(dgst == NULL) {
                xmlSecGCryptError("gcry_md_read", (gcry_error_t)GPG_ERR_NO_ERROR,
                                  xmlSecTransformGetName(transform));
                return(-1);
            }
            memcpy(ctx->dgst, dgst, XMLSEC_TRANSFORM_HMAC_BITS_TO_BYTES(ctx->dgstSizeInBits));

            /* write results if needed */
            if(transform->operation == xmlSecTransformOperationSign) {
                ret = xmlSecTransformHmacWriteOutput(ctx->dgst, ctx->dgstSizeInBits, XMLSEC_TRANSFORM_HMAC_MAX_OUTPUT_SIZE, out);
                if(ret < 0) {
                    xmlSecInternalError("xmlSecTransformHmacWriteOutput", xmlSecTransformGetName(transform));
                    return(-1);
                }
            }
            transform->status = xmlSecTransformStatusFinished;
        }
    } else if(transform->status == xmlSecTransformStatusFinished) {
        /* the only way we can get here is if there is no input */
        xmlSecAssert2(xmlSecBufferGetSize(&(transform->inBuf)) == 0, -1);
    } else {
        xmlSecInvalidTransfromStatusError(transform);
        return(-1);
    }

    return(0);
}

#ifndef XMLSEC_NO_SHA1
/******************************************************************************
 *
 * HMAC SHA1
 *
 ******************************************************************************/
static xmlSecTransformKlass xmlSecGCryptHmacSha1Klass = {
    /* klass/object sizes */
    sizeof(xmlSecTransformKlass),               /* xmlSecSize klassSize */
    xmlSecGCryptHmacSize,                       /* xmlSecSize objSize */

    xmlSecNameHmacSha1,                         /* const xmlChar* name; */
    xmlSecHrefHmacSha1,                         /* const xmlChar *href; */
    xmlSecTransformUsageSignatureMethod,        /* xmlSecTransformUsage usage; */

    xmlSecGCryptHmacInitialize,                 /* xmlSecTransformInitializeMethod initialize; */
    xmlSecGCryptHmacFinalize,                   /* xmlSecTransformFinalizeMethod finalize; */
    xmlSecGCryptHmacNodeRead,                   /* xmlSecTransformNodeReadMethod readNode; */
    NULL,                                       /* xmlSecTransformNodeWriteMethod writeNode; */
    xmlSecGCryptHmacSetKeyReq,                  /* xmlSecTransformSetKeyReqMethod setKeyReq; */
    xmlSecGCryptHmacSetKey,                     /* xmlSecTransformSetKeyMethod setKey; */
    xmlSecGCryptHmacVerify,                     /* xmlSecTransformValidateMethod validate; */
    xmlSecTransformDefaultGetDataType,          /* xmlSecTransformGetDataTypeMethod getDataType; */
    xmlSecTransformDefaultPushBin,              /* xmlSecTransformPushBinMethod pushBin; */
    xmlSecTransformDefaultPopBin,               /* xmlSecTransformPopBinMethod popBin; */
    NULL,                                       /* xmlSecTransformPushXmlMethod pushXml; */
    NULL,                                       /* xmlSecTransformPopXmlMethod popXml; */
    xmlSecGCryptHmacExecute,                    /* xmlSecTransformExecuteMethod execute; */

    NULL,                                       /* void* reserved0; */
    NULL,                                       /* void* reserved1; */
};

/**
 * xmlSecGCryptTransformHmacSha1GetKlass:
 *
 * The HMAC-SHA1 transform klass.
 *
 * Returns: the HMAC-SHA1 transform klass.
 */
xmlSecTransformId
xmlSecGCryptTransformHmacSha1GetKlass(void) {
    return(&xmlSecGCryptHmacSha1Klass);
}
#endif /* XMLSEC_NO_SHA1 */

#ifndef XMLSEC_NO_SHA256
/******************************************************************************
 *
 * HMAC SHA256
 *
 ******************************************************************************/
static xmlSecTransformKlass xmlSecGCryptHmacSha256Klass = {
    /* klass/object sizes */
    sizeof(xmlSecTransformKlass),               /* xmlSecSize klassSize */
    xmlSecGCryptHmacSize,                       /* xmlSecSize objSize */

    xmlSecNameHmacSha256,                       /* const xmlChar* name; */
    xmlSecHrefHmacSha256,                       /* const xmlChar *href; */
    xmlSecTransformUsageSignatureMethod,        /* xmlSecTransformUsage usage; */

    xmlSecGCryptHmacInitialize,                 /* xmlSecTransformInitializeMethod initialize; */
    xmlSecGCryptHmacFinalize,                   /* xmlSecTransformFinalizeMethod finalize; */
    xmlSecGCryptHmacNodeRead,                   /* xmlSecTransformNodeReadMethod readNode; */
    NULL,                                       /* xmlSecTransformNodeWriteMethod writeNode; */
    xmlSecGCryptHmacSetKeyReq,                  /* xmlSecTransformSetKeyReqMethod setKeyReq; */
    xmlSecGCryptHmacSetKey,                     /* xmlSecTransformSetKeyMethod setKey; */
    xmlSecGCryptHmacVerify,                     /* xmlSecTransformValidateMethod validate; */
    xmlSecTransformDefaultGetDataType,          /* xmlSecTransformGetDataTypeMethod getDataType; */
    xmlSecTransformDefaultPushBin,              /* xmlSecTransformPushBinMethod pushBin; */
    xmlSecTransformDefaultPopBin,               /* xmlSecTransformPopBinMethod popBin; */
    NULL,                                       /* xmlSecTransformPushXmlMethod pushXml; */
    NULL,                                       /* xmlSecTransformPopXmlMethod popXml; */
    xmlSecGCryptHmacExecute,                    /* xmlSecTransformExecuteMethod execute; */

    NULL,                                       /* void* reserved0; */
    NULL,                                       /* void* reserved1; */
};

/**
 * xmlSecGCryptTransformHmacSha256GetKlass:
 *
 * The HMAC-SHA256 transform klass.
 *
 * Returns: the HMAC-SHA256 transform klass.
 */
xmlSecTransformId
xmlSecGCryptTransformHmacSha256GetKlass(void) {
    return(&xmlSecGCryptHmacSha256Klass);
}
#endif /* XMLSEC_NO_SHA256 */

#ifndef XMLSEC_NO_SHA384
/******************************************************************************
 *
 * HMAC SHA384
 *
 ******************************************************************************/
static xmlSecTransformKlass xmlSecGCryptHmacSha384Klass = {
    /* klass/object sizes */
    sizeof(xmlSecTransformKlass),               /* xmlSecSize klassSize */
    xmlSecGCryptHmacSize,                       /* xmlSecSize objSize */

    xmlSecNameHmacSha384,                       /* const xmlChar* name; */
    xmlSecHrefHmacSha384,                       /* const xmlChar *href; */
    xmlSecTransformUsageSignatureMethod,        /* xmlSecTransformUsage usage; */

    xmlSecGCryptHmacInitialize,                 /* xmlSecTransformInitializeMethod initialize; */
    xmlSecGCryptHmacFinalize,                   /* xmlSecTransformFinalizeMethod finalize; */
    xmlSecGCryptHmacNodeRead,                   /* xmlSecTransformNodeReadMethod readNode; */
    NULL,                                       /* xmlSecTransformNodeWriteMethod writeNode; */
    xmlSecGCryptHmacSetKeyReq,                  /* xmlSecTransformSetKeyReqMethod setKeyReq; */
    xmlSecGCryptHmacSetKey,                     /* xmlSecTransformSetKeyMethod setKey; */
    xmlSecGCryptHmacVerify,                     /* xmlSecTransformValidateMethod validate; */
    xmlSecTransformDefaultGetDataType,          /* xmlSecTransformGetDataTypeMethod getDataType; */
    xmlSecTransformDefaultPushBin,              /* xmlSecTransformPushBinMethod pushBin; */
    xmlSecTransformDefaultPopBin,               /* xmlSecTransformPopBinMethod popBin; */
    NULL,                                       /* xmlSecTransformPushXmlMethod pushXml; */
    NULL,                                       /* xmlSecTransformPopXmlMethod popXml; */
    xmlSecGCryptHmacExecute,                    /* xmlSecTransformExecuteMethod execute; */

    NULL,                                       /* void* reserved0; */
    NULL,                                       /* void* reserved1; */
};

/**
 * xmlSecGCryptTransformHmacSha384GetKlass:
 *
 * The HMAC-SHA384 transform klass.
 *
 * Returns: the HMAC-SHA384 transform klass.
 */
xmlSecTransformId
xmlSecGCryptTransformHmacSha384GetKlass(void) {
    return(&xmlSecGCryptHmacSha384Klass);
}
#endif /* XMLSEC_NO_SHA384 */

#ifndef XMLSEC_NO_SHA512
/******************************************************************************
 *
 * HMAC SHA512
 *
 ******************************************************************************/
static xmlSecTransformKlass xmlSecGCryptHmacSha512Klass = {
    /* klass/object sizes */
    sizeof(xmlSecTransformKlass),               /* xmlSecSize klassSize */
    xmlSecGCryptHmacSize,                       /* xmlSecSize objSize */

    xmlSecNameHmacSha512,                       /* const xmlChar* name; */
    xmlSecHrefHmacSha512,                       /* const xmlChar *href; */
    xmlSecTransformUsageSignatureMethod,        /* xmlSecTransformUsage usage; */

    xmlSecGCryptHmacInitialize,                 /* xmlSecTransformInitializeMethod initialize; */
    xmlSecGCryptHmacFinalize,                   /* xmlSecTransformFinalizeMethod finalize; */
    xmlSecGCryptHmacNodeRead,                   /* xmlSecTransformNodeReadMethod readNode; */
    NULL,                                       /* xmlSecTransformNodeWriteMethod writeNode; */
    xmlSecGCryptHmacSetKeyReq,                  /* xmlSecTransformSetKeyReqMethod setKeyReq; */
    xmlSecGCryptHmacSetKey,                     /* xmlSecTransformSetKeyMethod setKey; */
    xmlSecGCryptHmacVerify,                     /* xmlSecTransformValidateMethod validate; */
    xmlSecTransformDefaultGetDataType,          /* xmlSecTransformGetDataTypeMethod getDataType; */
    xmlSecTransformDefaultPushBin,              /* xmlSecTransformPushBinMethod pushBin; */
    xmlSecTransformDefaultPopBin,               /* xmlSecTransformPopBinMethod popBin; */
    NULL,                                       /* xmlSecTransformPushXmlMethod pushXml; */
    NULL,                                       /* xmlSecTransformPopXmlMethod popXml; */
    xmlSecGCryptHmacExecute,                    /* xmlSecTransformExecuteMethod execute; */

    NULL,                                       /* void* reserved0; */
    NULL,                                       /* void* reserved1; */
};

/**
 * xmlSecGCryptTransformHmacSha512GetKlass:
 *
 * The HMAC-SHA512 transform klass.
 *
 * Returns: the HMAC-SHA512 transform klass.
 */
xmlSecTransformId
xmlSecGCryptTransformHmacSha512GetKlass(void) {
    return(&xmlSecGCryptHmacSha512Klass);
}
#endif /* XMLSEC_NO_SHA512 */


#ifndef XMLSEC_NO_RIPEMD160
/******************************************************************************
 *
 * HMAC Ripemd160
 *
 ******************************************************************************/
static xmlSecTransformKlass xmlSecGCryptHmacRipemd160Klass = {
    /* klass/object sizes */
    sizeof(xmlSecTransformKlass),               /* xmlSecSize klassSize */
    xmlSecGCryptHmacSize,                       /* xmlSecSize objSize */

    xmlSecNameHmacRipemd160,                    /* const xmlChar* name; */
    xmlSecHrefHmacRipemd160,                    /* const xmlChar* href; */
    xmlSecTransformUsageSignatureMethod,        /* xmlSecTransformUsage usage; */

    xmlSecGCryptHmacInitialize,                 /* xmlSecTransformInitializeMethod initialize; */
    xmlSecGCryptHmacFinalize,                   /* xmlSecTransformFinalizeMethod finalize; */
    xmlSecGCryptHmacNodeRead,                   /* xmlSecTransformNodeReadMethod readNode; */
    NULL,                                       /* xmlSecTransformNodeWriteMethod writeNode; */
    xmlSecGCryptHmacSetKeyReq,                  /* xmlSecTransformSetKeyReqMethod setKeyReq; */
    xmlSecGCryptHmacSetKey,                     /* xmlSecTransformSetKeyMethod setKey; */
    xmlSecGCryptHmacVerify,                     /* xmlSecTransformValidateMethod validate; */
    xmlSecTransformDefaultGetDataType,          /* xmlSecTransformGetDataTypeMethod getDataType; */
    xmlSecTransformDefaultPushBin,              /* xmlSecTransformPushBinMethod pushBin; */
    xmlSecTransformDefaultPopBin,               /* xmlSecTransformPopBinMethod popBin; */
    NULL,                                       /* xmlSecTransformPushXmlMethod pushXml; */
    NULL,                                       /* xmlSecTransformPopXmlMethod popXml; */
    xmlSecGCryptHmacExecute,                    /* xmlSecTransformExecuteMethod execute; */

    NULL,                                       /* void* reserved0; */
    NULL,                                       /* void* reserved1; */
};

/**
 * xmlSecGCryptTransformHmacRipemd160GetKlass:
 *
 * The HMAC-RIPEMD160 transform klass.
 *
 * Returns: the HMAC-RIPEMD160 transform klass.
 */
xmlSecTransformId
xmlSecGCryptTransformHmacRipemd160GetKlass(void) {
    return(&xmlSecGCryptHmacRipemd160Klass);
}
#endif /* XMLSEC_NO_RIPEMD160 */

#ifndef XMLSEC_NO_MD5
/******************************************************************************
 *
 * HMAC MD5
 *
 ******************************************************************************/
static xmlSecTransformKlass xmlSecGCryptHmacMd5Klass = {
    /* klass/object sizes */
    sizeof(xmlSecTransformKlass),               /* xmlSecSize klassSize */
    xmlSecGCryptHmacSize,                       /* xmlSecSize objSize */

    xmlSecNameHmacMd5,                          /* const xmlChar* name; */
    xmlSecHrefHmacMd5,                          /* const xmlChar* href; */
    xmlSecTransformUsageSignatureMethod,        /* xmlSecTransformUsage usage; */

    xmlSecGCryptHmacInitialize,                 /* xmlSecTransformInitializeMethod initialize; */
    xmlSecGCryptHmacFinalize,                   /* xmlSecTransformFinalizeMethod finalize; */
    xmlSecGCryptHmacNodeRead,                   /* xmlSecTransformNodeReadMethod readNode; */
    NULL,                                       /* xmlSecTransformNodeWriteMethod writeNode; */
    xmlSecGCryptHmacSetKeyReq,                  /* xmlSecTransformSetKeyReqMethod setKeyReq; */
    xmlSecGCryptHmacSetKey,                     /* xmlSecTransformSetKeyMethod setKey; */
    xmlSecGCryptHmacVerify,                     /* xmlSecTransformValidateMethod validate; */
    xmlSecTransformDefaultGetDataType,          /* xmlSecTransformGetDataTypeMethod getDataType; */
    xmlSecTransformDefaultPushBin,              /* xmlSecTransformPushBinMethod pushBin; */
    xmlSecTransformDefaultPopBin,               /* xmlSecTransformPopBinMethod popBin; */
    NULL,                                       /* xmlSecTransformPushXmlMethod pushXml; */
    NULL,                                       /* xmlSecTransformPopXmlMethod popXml; */
    xmlSecGCryptHmacExecute,                    /* xmlSecTransformExecuteMethod execute; */

    NULL,                                       /* void* reserved0; */
    NULL,                                       /* void* reserved1; */
};

/**
 * xmlSecGCryptTransformHmacMd5GetKlass:
 *
 * The HMAC-MD5 transform klass.
 *
 * Returns: the HMAC-MD5 transform klass.
 */
xmlSecTransformId
xmlSecGCryptTransformHmacMd5GetKlass(void) {
    return(&xmlSecGCryptHmacMd5Klass);
}
#endif /* XMLSEC_NO_MD5 */


#endif /* XMLSEC_NO_HMAC */
