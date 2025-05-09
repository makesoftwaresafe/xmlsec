/*
 * XML Security Library (http://www.aleksey.com/xmlsec).
 *
 * HMAC transforms implementation for MSCng.
 *
 * This is free software; see Copyright file in the source
 * distribution for preciese wording.
 *
 * Copyright (C) 2018 Miklos Vajna. All Rights Reserved.
 */
/**
 * SECTION:crypto
 */

#ifndef XMLSEC_NO_HMAC
#include "globals.h"

#include <string.h>

#include <xmlsec/xmlsec.h>
#include <xmlsec/errors.h>
#include <xmlsec/keys.h>
#include <xmlsec/keyinfo.h>
#include <xmlsec/transforms.h>
#include <xmlsec/private.h>

#include <xmlsec/mscng/crypto.h>

#include "../cast_helpers.h"
#include "../keysdata_helpers.h"
#include "../transform_helpers.h"

typedef struct _xmlSecMSCngHmacCtx xmlSecMSCngHmacCtx, *xmlSecMSCngHmacCtxPtr;

struct _xmlSecMSCngHmacCtx {
    LPCWSTR pszAlgId;
    int initialized;
    BCRYPT_ALG_HANDLE hAlg;
    PBYTE hash;
    DWORD hashLength;
    xmlSecSize dgstSizeInBits; /* truncation length in bits */
    BCRYPT_HASH_HANDLE hHash;
};

XMLSEC_TRANSFORM_DECLARE(MSCngHmac, xmlSecMSCngHmacCtx)
#define xmlSecMSCngHmacSize XMLSEC_TRANSFORM_SIZE(MSCngHmac)

/* 80 is a minimum value from: https://www.w3.org/TR/xmldsig-core1/#sec-SignatureMethod */
#define XMLSEC_MSCNG_HMAC_MIN_LENGTH                     80

static int
xmlSecMSCngHmacCheckId(xmlSecTransformPtr transform) {

#ifndef XMLSEC_NO_MD5
    if(xmlSecTransformCheckId(transform, xmlSecMSCngTransformHmacMd5Id)) {
        return(1);
    } else
#endif /* XMLSEC_NO_MD5 */

#ifndef XMLSEC_NO_SHA1
    if(xmlSecTransformCheckId(transform, xmlSecMSCngTransformHmacSha1Id)) {
        return(1);
    } else
#endif /* XMLSEC_NO_SHA1 */

#ifndef XMLSEC_NO_SHA256
    if(xmlSecTransformCheckId(transform, xmlSecMSCngTransformHmacSha256Id)) {
        return(1);
    } else
#endif /* XMLSEC_NO_SHA256 */

#ifndef XMLSEC_NO_SHA384
    if(xmlSecTransformCheckId(transform, xmlSecMSCngTransformHmacSha384Id)) {
        return(1);
    } else
#endif /* XMLSEC_NO_SHA384 */

#ifndef XMLSEC_NO_SHA512
    if(xmlSecTransformCheckId(transform, xmlSecMSCngTransformHmacSha512Id)) {
        return(1);
    } else
#endif /* XMLSEC_NO_SHA512 */

    /* not found */
    {
        return(0);
    }
}
static int
xmlSecMSCngHmacInitialize(xmlSecTransformPtr transform) {
    xmlSecMSCngHmacCtxPtr ctx;

    xmlSecAssert2(xmlSecMSCngHmacCheckId(transform), -1);
    xmlSecAssert2(xmlSecTransformCheckSize(transform, xmlSecMSCngHmacSize), -1);

    ctx = xmlSecMSCngHmacGetCtx(transform);
    xmlSecAssert2(ctx != NULL, -1);

    /* initialize context */
    memset(ctx, 0, sizeof(xmlSecMSCngHmacCtx));

#ifndef XMLSEC_NO_MD5
    if(xmlSecTransformCheckId(transform, xmlSecMSCngTransformHmacMd5Id)) {
        ctx->pszAlgId = BCRYPT_MD5_ALGORITHM;
    } else
#endif /* XMLSEC_NO_MD5 */

#ifndef XMLSEC_NO_SHA1
    if(xmlSecTransformCheckId(transform, xmlSecMSCngTransformHmacSha1Id)) {
        ctx->pszAlgId = BCRYPT_SHA1_ALGORITHM;
    } else
#endif /* XMLSEC_NO_SHA1 */

#ifndef XMLSEC_NO_SHA256
    if(xmlSecTransformCheckId(transform, xmlSecMSCngTransformHmacSha256Id)) {
        ctx->pszAlgId = BCRYPT_SHA256_ALGORITHM;
    } else
#endif /* XMLSEC_NO_SHA256 */

#ifndef XMLSEC_NO_SHA384
    if(xmlSecTransformCheckId(transform, xmlSecMSCngTransformHmacSha384Id)) {
        ctx->pszAlgId = BCRYPT_SHA384_ALGORITHM;
    } else
#endif /* XMLSEC_NO_SHA384 */

#ifndef XMLSEC_NO_SHA512
    if(xmlSecTransformCheckId(transform, xmlSecMSCngTransformHmacSha512Id)) {
        ctx->pszAlgId = BCRYPT_SHA512_ALGORITHM;
    } else
#endif /* XMLSEC_NO_SHA512 */

    /* not found */
    {
        xmlSecInvalidTransfromError(transform)
        return(-1);
    }

    return(0);
}

static void
xmlSecMSCngHmacFinalize(xmlSecTransformPtr transform) {
    xmlSecMSCngHmacCtxPtr ctx;

    xmlSecAssert(xmlSecMSCngHmacCheckId(transform));
    xmlSecAssert(xmlSecTransformCheckSize(transform, xmlSecMSCngHmacSize));

    ctx = xmlSecMSCngHmacGetCtx(transform);
    xmlSecAssert(ctx != NULL);

    if(ctx->hash != NULL) {
        xmlFree(ctx->hash);
    }

    if(ctx->hHash != NULL) {
        BCryptDestroyHash(ctx->hHash);
    }

    if(ctx->hAlg != NULL) {
        BCryptCloseAlgorithmProvider(ctx->hAlg, 0);
    }

    memset(ctx, 0, sizeof(xmlSecMSCngHmacCtx));
}

static int
xmlSecMSCngHmacNodeRead(xmlSecTransformPtr transform, xmlNodePtr node,
                        xmlSecTransformCtxPtr transformCtx XMLSEC_ATTRIBUTE_UNUSED) {
    xmlSecMSCngHmacCtxPtr ctx;
    int ret;

    xmlSecAssert2(xmlSecMSCngHmacCheckId(transform), -1);
    xmlSecAssert2(xmlSecTransformCheckSize(transform, xmlSecMSCngHmacSize), -1);
    xmlSecAssert2(node!= NULL, -1);
    UNREFERENCED_PARAMETER(transformCtx);

    ctx = xmlSecMSCngHmacGetCtx(transform);
    xmlSecAssert2(ctx != NULL, -1);

    ret = xmlSecTransformHmacReadOutputBitsSize(node, ctx->dgstSizeInBits, &ctx->dgstSizeInBits);
    if (ret < 0) {
        xmlSecInternalError("xmlSecTransformHmacReadOutputBitsSize()",
            xmlSecTransformGetName(transform));
        return(-1);
    }

    return(0);
}

static int
xmlSecMSCngHmacSetKeyReq(xmlSecTransformPtr transform,  xmlSecKeyReqPtr keyReq) {
    xmlSecAssert2(xmlSecMSCngHmacCheckId(transform), -1);
    xmlSecAssert2((transform->operation == xmlSecTransformOperationSign) || (transform->operation == xmlSecTransformOperationVerify), -1);
    xmlSecAssert2(xmlSecTransformCheckSize(transform, xmlSecMSCngHmacSize), -1);
    xmlSecAssert2(keyReq != NULL, -1);

    keyReq->keyId = xmlSecMSCngKeyDataHmacId;
    keyReq->keyType = xmlSecKeyDataTypeSymmetric;
    if(transform->operation == xmlSecTransformOperationSign) {
        keyReq->keyUsage = xmlSecKeyUsageSign;
    } else {
        keyReq->keyUsage = xmlSecKeyUsageVerify;
    }

    return(0);
}

static int
xmlSecMSCngHmacSetKey(xmlSecTransformPtr transform, xmlSecKeyPtr key) {
    xmlSecMSCngHmacCtxPtr ctx;
    xmlSecKeyDataPtr value;
    xmlSecBufferPtr buffer;
    xmlSecSize bufSize;
    DWORD dwBufSize, resultLength = 0;
    NTSTATUS status;

    xmlSecAssert2(xmlSecMSCngHmacCheckId(transform), -1);
    xmlSecAssert2((transform->operation == xmlSecTransformOperationSign) || (transform->operation == xmlSecTransformOperationVerify), -1);
    xmlSecAssert2(xmlSecTransformCheckSize(transform, xmlSecMSCngHmacSize), -1);
    xmlSecAssert2(key != NULL, -1);

    ctx = xmlSecMSCngHmacGetCtx(transform);
    xmlSecAssert2(ctx != NULL, -1);
    xmlSecAssert2(ctx->initialized == 0, -1);

    value = xmlSecKeyGetValue(key);
    xmlSecAssert2(xmlSecKeyDataCheckId(value, xmlSecMSCngKeyDataHmacId), -1);

    buffer = xmlSecKeyDataBinaryValueGetBuffer(value);
    xmlSecAssert2(buffer != NULL, -1);

    if(xmlSecBufferGetSize(buffer) == 0) {
        xmlSecInvalidZeroKeyDataSizeError(xmlSecTransformGetName(transform));
        return(-1);
    }

    xmlSecAssert2(xmlSecBufferGetData(buffer) != NULL, -1);

    /* at this point we know what should be they key, go ahead with the CNG
     * calls */

    status = BCryptOpenAlgorithmProvider(&ctx->hAlg,
        ctx->pszAlgId,
        NULL,
        BCRYPT_ALG_HANDLE_HMAC_FLAG);
    if(status != STATUS_SUCCESS) {
        xmlSecMSCngNtError("BCryptOpenAlgorithmProvider", xmlSecTransformGetName(transform), status);
        return(-1);
    }

    status = BCryptGetProperty(ctx->hAlg,
        BCRYPT_HASH_LENGTH,
        (PBYTE)&ctx->hashLength,
        sizeof(ctx->hashLength),
        &resultLength,
        0);
    if(status != STATUS_SUCCESS) {
        xmlSecMSCngNtError("BCryptGetProperty", xmlSecTransformGetName(transform), status);
        return(-1);
    }

    ctx->hash = (PBYTE)xmlMalloc(ctx->hashLength);
    if(ctx->hash == NULL) {
        xmlSecMallocError(ctx->hashLength, NULL);
        return(-1);
    }

    bufSize = xmlSecBufferGetSize(buffer);
    XMLSEC_SAFE_CAST_SIZE_TO_ULONG(bufSize, dwBufSize, return(-1), xmlSecTransformGetName(transform));
    status = BCryptCreateHash(ctx->hAlg,
        &ctx->hHash,
        NULL,
        0,
        (PBYTE)xmlSecBufferGetData(buffer),
        dwBufSize,
        0);
    if(status != STATUS_SUCCESS) {
        xmlSecMSCngNtError("BCryptCreateHash", xmlSecTransformGetName(transform), status);
        return(-1);
    }

    if (ctx->dgstSizeInBits == 0) {
        /* no custom value is requested, then default to the full length */
        ctx->dgstSizeInBits = ctx->hashLength * 8;
    }

    ctx->initialized = 1;
    return(0);
}

static int
xmlSecMSCngHmacVerify(xmlSecTransformPtr transform, const xmlSecByte* data,
        xmlSecSize dataSize, xmlSecTransformCtxPtr transformCtx XMLSEC_ATTRIBUTE_UNUSED) {
    xmlSecMSCngHmacCtxPtr ctx;
    int ret;

    xmlSecAssert2(xmlSecTransformIsValid(transform), -1);
    xmlSecAssert2(xmlSecTransformCheckSize(transform, xmlSecMSCngHmacSize), -1);
    xmlSecAssert2(transform->operation == xmlSecTransformOperationVerify, -1);
    xmlSecAssert2(transform->status == xmlSecTransformStatusFinished, -1);
    xmlSecAssert2(data != NULL, -1);
    xmlSecAssert2(dataSize > 0, -1);
    UNREFERENCED_PARAMETER(transformCtx);

    ctx = xmlSecMSCngHmacGetCtx(transform);
    xmlSecAssert2(ctx != NULL, -1);
    xmlSecAssert2(ctx->dgstSizeInBits > 0, -1);

    /* Returns 1 for match, 0 for no match, <0 for errors. */
    ret = xmlSecTransformHmacVerify(data, dataSize, ctx->hash, ctx->dgstSizeInBits, ctx->hashLength);
    if (ret < 0) {
        xmlSecInternalError("xmlSecTransformHmacVerify", xmlSecTransformGetName(transform));
        return(-1);
    }
    if (ret == 1) {
        transform->status = xmlSecTransformStatusOk;
    }
    else {
        transform->status = xmlSecTransformStatusFail;
    }
    return(0);
}

static int
xmlSecMSCngHmacExecute(xmlSecTransformPtr transform, int last, xmlSecTransformCtxPtr transformCtx) {
    xmlSecMSCngHmacCtxPtr ctx;
    xmlSecBufferPtr in, out;
    NTSTATUS status;
    int ret;

    xmlSecAssert2(xmlSecTransformIsValid(transform), -1);
    xmlSecAssert2((transform->operation == xmlSecTransformOperationSign) || (transform->operation == xmlSecTransformOperationVerify), -1);
    xmlSecAssert2(xmlSecTransformCheckSize(transform, xmlSecMSCngHmacSize), -1);
    xmlSecAssert2(transformCtx != NULL, -1);

    in = &(transform->inBuf);
    out = &(transform->outBuf);

    ctx = xmlSecMSCngHmacGetCtx(transform);
    xmlSecAssert2(ctx != NULL, -1);
    xmlSecAssert2(ctx->initialized != 0, -1);

    if(transform->status == xmlSecTransformStatusNone) {
        /* we should be already initialized when we set key */
        transform->status = xmlSecTransformStatusWorking;
    }

    if(transform->status == xmlSecTransformStatusWorking) {
        xmlSecSize inSize;

        inSize = xmlSecBufferGetSize(in);
        if(inSize > 0) {
            DWORD dwInSize;

            XMLSEC_SAFE_CAST_SIZE_TO_ULONG(inSize, dwInSize, return(-1), xmlSecTransformGetName(transform));
            status = BCryptHashData(ctx->hHash,
                xmlSecBufferGetData(in),
                dwInSize,
                0);
            if(status != STATUS_SUCCESS) {
                xmlSecMSCngNtError("BCryptHashData", xmlSecTransformGetName(transform), status);
                return(-1);
            }

            ret = xmlSecBufferRemoveHead(in, inSize);
            if(ret < 0) {
                xmlSecInternalError2("xmlSecBufferRemoveHead", xmlSecTransformGetName(transform),
                    "size=" XMLSEC_SIZE_FMT, inSize);
                return(-1);
            }
        }

        if(last) {
            status = BCryptFinishHash(ctx->hHash,
                ctx->hash,
                ctx->hashLength,
                0);
            if(status != STATUS_SUCCESS) {
                xmlSecMSCngNtError("BCryptFinishHash", xmlSecTransformGetName(transform), status);
                return(-1);
            }

            /* write results if needed */
            if (transform->operation == xmlSecTransformOperationSign) {
                ret = xmlSecTransformHmacWriteOutput(ctx->hash, ctx->dgstSizeInBits, ctx->hashLength, out);
                if (ret < 0) {
                    xmlSecInternalError("xmlSecTransformHmacWriteOutput", xmlSecTransformGetName(transform));
                    return(-1);
                }
            }
            transform->status = xmlSecTransformStatusFinished;
        }
    } else if(transform->status == xmlSecTransformStatusFinished) {
        /* the only way we can get here is if there is no input */
        xmlSecAssert2(xmlSecBufferGetSize(in) == 0, -1);
    } else {
        xmlSecInvalidTransfromStatusError(transform);
        return(-1);
    }

    return(0);
}

#ifndef XMLSEC_NO_MD5
/******************************************************************************
 *
 * HMAC MD5
 *
 ******************************************************************************/
static xmlSecTransformKlass xmlSecMSCngHmacMd5Klass = {
    /* klass/object sizes */
    sizeof(xmlSecTransformKlass),               /* xmlSecSize klassSize */
    xmlSecMSCngHmacSize,                        /* xmlSecSize objSize */

    xmlSecNameHmacMd5,                          /* const xmlChar* name; */
    xmlSecHrefHmacMd5,                          /* const xmlChar* href; */
    xmlSecTransformUsageSignatureMethod,        /* xmlSecTransformUsage usage; */

    xmlSecMSCngHmacInitialize,                  /* xmlSecTransformInitializeMethod initialize; */
    xmlSecMSCngHmacFinalize,                    /* xmlSecTransformFinalizeMethod finalize; */
    xmlSecMSCngHmacNodeRead,                    /* xmlSecTransformNodeReadMethod readNode; */
    NULL,                                       /* xmlSecTransformNodeWriteMethod writeNode; */
    xmlSecMSCngHmacSetKeyReq,                   /* xmlSecTransformSetKeyReqMethod setKeyReq; */
    xmlSecMSCngHmacSetKey,                      /* xmlSecTransformSetKeyMethod setKey; */
    xmlSecMSCngHmacVerify,                      /* xmlSecTransformValidateMethod validate; */
    xmlSecTransformDefaultGetDataType,          /* xmlSecTransformGetDataTypeMethod getDataType; */
    xmlSecTransformDefaultPushBin,              /* xmlSecTransformPushBinMethod pushBin; */
    xmlSecTransformDefaultPopBin,               /* xmlSecTransformPopBinMethod popBin; */
    NULL,                                       /* xmlSecTransformPushXmlMethod pushXml; */
    NULL,                                       /* xmlSecTransformPopXmlMethod popXml; */
    xmlSecMSCngHmacExecute,                     /* xmlSecTransformExecuteMethod execute; */

    NULL,                                       /* void* reserved0; */
    NULL,                                       /* void* reserved1; */
};

/**
 * xmlSecMSCngTransformHmacMd5GetKlass:
 *
 * The HMAC-MD5 transform klass.
 *
 * Returns: the HMAC-MD5 transform klass.
 */
xmlSecTransformId
xmlSecMSCngTransformHmacMd5GetKlass(void) {
    return(&xmlSecMSCngHmacMd5Klass);
}

#endif /* XMLSEC_NO_MD5 */

#ifndef XMLSEC_NO_SHA1
/******************************************************************************
 *
 * HMAC SHA1
 *
 ******************************************************************************/
static xmlSecTransformKlass xmlSecMSCngHmacSha1Klass = {
    /* klass/object sizes */
    sizeof(xmlSecTransformKlass),               /* xmlSecSize klassSize */
    xmlSecMSCngHmacSize,                        /* xmlSecSize objSize */

    xmlSecNameHmacSha1,                         /* const xmlChar* name; */
    xmlSecHrefHmacSha1,                         /* const xmlChar* href; */
    xmlSecTransformUsageSignatureMethod,        /* xmlSecTransformUsage usage; */

    xmlSecMSCngHmacInitialize,                  /* xmlSecTransformInitializeMethod initialize; */
    xmlSecMSCngHmacFinalize,                    /* xmlSecTransformFinalizeMethod finalize; */
    xmlSecMSCngHmacNodeRead,                    /* xmlSecTransformNodeReadMethod readNode; */
    NULL,                                       /* xmlSecTransformNodeWriteMethod writeNode; */
    xmlSecMSCngHmacSetKeyReq,                   /* xmlSecTransformSetKeyReqMethod setKeyReq; */
    xmlSecMSCngHmacSetKey,                      /* xmlSecTransformSetKeyMethod setKey; */
    xmlSecMSCngHmacVerify,                      /* xmlSecTransformValidateMethod validate; */
    xmlSecTransformDefaultGetDataType,          /* xmlSecTransformGetDataTypeMethod getDataType; */
    xmlSecTransformDefaultPushBin,              /* xmlSecTransformPushBinMethod pushBin; */
    xmlSecTransformDefaultPopBin,               /* xmlSecTransformPopBinMethod popBin; */
    NULL,                                       /* xmlSecTransformPushXmlMethod pushXml; */
    NULL,                                       /* xmlSecTransformPopXmlMethod popXml; */
    xmlSecMSCngHmacExecute,                     /* xmlSecTransformExecuteMethod execute; */

    NULL,                                       /* void* reserved0; */
    NULL,                                       /* void* reserved1; */
};

/**
 * xmlSecMSCngTransformHmacSha1GetKlass:
 *
 * The HMAC-SHA1 transform klass.
 *
 * Returns: the HMAC-SHA1 transform klass.
 */
xmlSecTransformId
xmlSecMSCngTransformHmacSha1GetKlass(void) {
    return(&xmlSecMSCngHmacSha1Klass);
}

#endif /* XMLSEC_NO_SHA1 */

#ifndef XMLSEC_NO_SHA256
/******************************************************************************
 *
 * HMAC SHA256
 *
 ******************************************************************************/
static xmlSecTransformKlass xmlSecMSCngHmacSha256Klass = {
    /* klass/object sizes */
    sizeof(xmlSecTransformKlass),               /* xmlSecSize klassSize */
    xmlSecMSCngHmacSize,                        /* xmlSecSize objSize */

    xmlSecNameHmacSha256,                       /* const xmlChar* name; */
    xmlSecHrefHmacSha256,                       /* const xmlChar* href; */
    xmlSecTransformUsageSignatureMethod,        /* xmlSecTransformUsage usage; */

    xmlSecMSCngHmacInitialize,                  /* xmlSecTransformInitializeMethod initialize; */
    xmlSecMSCngHmacFinalize,                    /* xmlSecTransformFinalizeMethod finalize; */
    xmlSecMSCngHmacNodeRead,                    /* xmlSecTransformNodeReadMethod readNode; */
    NULL,                                       /* xmlSecTransformNodeWriteMethod writeNode; */
    xmlSecMSCngHmacSetKeyReq,                   /* xmlSecTransformSetKeyReqMethod setKeyReq; */
    xmlSecMSCngHmacSetKey,                      /* xmlSecTransformSetKeyMethod setKey; */
    xmlSecMSCngHmacVerify,                      /* xmlSecTransformValidateMethod validate; */
    xmlSecTransformDefaultGetDataType,          /* xmlSecTransformGetDataTypeMethod getDataType; */
    xmlSecTransformDefaultPushBin,              /* xmlSecTransformPushBinMethod pushBin; */
    xmlSecTransformDefaultPopBin,               /* xmlSecTransformPopBinMethod popBin; */
    NULL,                                       /* xmlSecTransformPushXmlMethod pushXml; */
    NULL,                                       /* xmlSecTransformPopXmlMethod popXml; */
    xmlSecMSCngHmacExecute,                     /* xmlSecTransformExecuteMethod execute; */

    NULL,                                       /* void* reserved0; */
    NULL,                                       /* void* reserved1; */
};

/**
 * xmlSecMSCngTransformHmacSha256GetKlass:
 *
 * The HMAC-SHA256 transform klass.
 *
 * Returns: the HMAC-SHA256 transform klass.
 */
xmlSecTransformId
xmlSecMSCngTransformHmacSha256GetKlass(void) {
    return(&xmlSecMSCngHmacSha256Klass);
}

#endif /* XMLSEC_NO_SHA256 */

#ifndef XMLSEC_NO_SHA384
/******************************************************************************
 *
 * HMAC SHA384
 *
 ******************************************************************************/
static xmlSecTransformKlass xmlSecMSCngHmacSha384Klass = {
    /* klass/object sizes */
    sizeof(xmlSecTransformKlass),               /* xmlSecSize klassSize */
    xmlSecMSCngHmacSize,                        /* xmlSecSize objSize */

    xmlSecNameHmacSha384,                       /* const xmlChar* name; */
    xmlSecHrefHmacSha384,                       /* const xmlChar* href; */
    xmlSecTransformUsageSignatureMethod,        /* xmlSecTransformUsage usage; */

    xmlSecMSCngHmacInitialize,                  /* xmlSecTransformInitializeMethod initialize; */
    xmlSecMSCngHmacFinalize,                    /* xmlSecTransformFinalizeMethod finalize; */
    xmlSecMSCngHmacNodeRead,                    /* xmlSecTransformNodeReadMethod readNode; */
    NULL,                                       /* xmlSecTransformNodeWriteMethod writeNode; */
    xmlSecMSCngHmacSetKeyReq,                   /* xmlSecTransformSetKeyReqMethod setKeyReq; */
    xmlSecMSCngHmacSetKey,                      /* xmlSecTransformSetKeyMethod setKey; */
    xmlSecMSCngHmacVerify,                      /* xmlSecTransformValidateMethod validate; */
    xmlSecTransformDefaultGetDataType,          /* xmlSecTransformGetDataTypeMethod getDataType; */
    xmlSecTransformDefaultPushBin,              /* xmlSecTransformPushBinMethod pushBin; */
    xmlSecTransformDefaultPopBin,               /* xmlSecTransformPopBinMethod popBin; */
    NULL,                                       /* xmlSecTransformPushXmlMethod pushXml; */
    NULL,                                       /* xmlSecTransformPopXmlMethod popXml; */
    xmlSecMSCngHmacExecute,                     /* xmlSecTransformExecuteMethod execute; */

    NULL,                                       /* void* reserved0; */
    NULL,                                       /* void* reserved1; */
};

/**
 * xmlSecMSCngTransformHmacSha384GetKlass:
 *
 * The HMAC-SHA384 transform klass.
 *
 * Returns: the HMAC-SHA384 transform klass.
 */
xmlSecTransformId
xmlSecMSCngTransformHmacSha384GetKlass(void) {
    return(&xmlSecMSCngHmacSha384Klass);
}

#endif /* XMLSEC_NO_SHA384 */

#ifndef XMLSEC_NO_SHA512
/******************************************************************************
 *
 * HMAC SHA512
 *
 ******************************************************************************/
static xmlSecTransformKlass xmlSecMSCngHmacSha512Klass = {
    /* klass/object sizes */
    sizeof(xmlSecTransformKlass),               /* xmlSecSize klassSize */
    xmlSecMSCngHmacSize,                        /* xmlSecSize objSize */

    xmlSecNameHmacSha512,                       /* const xmlChar* name; */
    xmlSecHrefHmacSha512,                       /* const xmlChar* href; */
    xmlSecTransformUsageSignatureMethod,        /* xmlSecTransformUsage usage; */

    xmlSecMSCngHmacInitialize,                  /* xmlSecTransformInitializeMethod initialize; */
    xmlSecMSCngHmacFinalize,                    /* xmlSecTransformFinalizeMethod finalize; */
    xmlSecMSCngHmacNodeRead,                    /* xmlSecTransformNodeReadMethod readNode; */
    NULL,                                       /* xmlSecTransformNodeWriteMethod writeNode; */
    xmlSecMSCngHmacSetKeyReq,                   /* xmlSecTransformSetKeyReqMethod setKeyReq; */
    xmlSecMSCngHmacSetKey,                      /* xmlSecTransformSetKeyMethod setKey; */
    xmlSecMSCngHmacVerify,                      /* xmlSecTransformValidateMethod validate; */
    xmlSecTransformDefaultGetDataType,          /* xmlSecTransformGetDataTypeMethod getDataType; */
    xmlSecTransformDefaultPushBin,              /* xmlSecTransformPushBinMethod pushBin; */
    xmlSecTransformDefaultPopBin,               /* xmlSecTransformPopBinMethod popBin; */
    NULL,                                       /* xmlSecTransformPushXmlMethod pushXml; */
    NULL,                                       /* xmlSecTransformPopXmlMethod popXml; */
    xmlSecMSCngHmacExecute,                     /* xmlSecTransformExecuteMethod execute; */

    NULL,                                       /* void* reserved0; */
    NULL,                                       /* void* reserved1; */
};

/**
 * xmlSecMSCngTransformHmacSha512GetKlass:
 *
 * The HMAC-SHA512 transform klass.
 *
 * Returns: the HMAC-SHA512 transform klass.
 */
xmlSecTransformId
xmlSecMSCngTransformHmacSha512GetKlass(void) {
    return(&xmlSecMSCngHmacSha512Klass);
}

#endif /* XMLSEC_NO_SHA512 */

#endif /* XMLSEC_NO_HMAC */
