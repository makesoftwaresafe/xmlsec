/** 
 * XML Security Library
 *
 * Encryption Algorithms: RIPEMD160
 * 
 * See Copyright for the status of this software.
 * 
 * Author: Aleksey Sanin <aleksey@aleksey.com>
 */
#include "globals.h"

#ifndef XMLSEC_NO_RIPEMD160

#include <stdlib.h>
#include <string.h>

#include <openssl/ripemd.h>

#include <xmlsec/xmlsec.h>
#include <xmlsec/keys.h>
#include <xmlsec/transforms.h>
#include <xmlsec/transformsInternal.h>
#include <xmlsec/digests.h>
#include <xmlsec/errors.h>

static xmlSecTransformPtr xmlSecDigestRipemd160Create(xmlSecTransformId id);
static void 	xmlSecDigestRipemd160Destroy	(xmlSecTransformPtr transform);
static int 	xmlSecDigestRipemd160Update	(xmlSecDigestTransformPtr transform,
						 const unsigned char *buffer,
						 size_t size);
static int 	xmlSecDigestRipemd160Sign	(xmlSecDigestTransformPtr transform,
						 unsigned char **buffer,
						 size_t *size);
static int 	xmlSecDigestRipemd160Verify	(xmlSecDigestTransformPtr transform,
						 const unsigned char *buffer,
						 size_t size);



struct _xmlSecDigestTransformIdStruct xmlSecDigestRipemd160Id = {
    /* same as xmlSecTransformId */    
    BAD_CAST "dgst-ripemd160",
    xmlSecTransformTypeBinary,		/* xmlSecTransformType type; */
    xmlSecTransformUsageDigestMethod,		/* xmlSecTransformUsage usage; */
    BAD_CAST "http://www.w3.org/2001/04/xmlenc#ripemd160", /* xmlChar *href; */
    
    xmlSecDigestRipemd160Create,	/* xmlSecTransformCreateMethod create; */
    xmlSecDigestRipemd160Destroy,	/* xmlSecTransformDestroyMethod destroy; */
    NULL,				/* xmlSecTransformReadNodeMethod read; */
    NULL,				/* xmlSecTransformSetKeyReqMethod setKeyReq; */
    NULL,				/* xmlSecTransformSetKeyMethod setKey; */
    NULL,				/* xmlSecTransformValidateMethod validate; */
    NULL,				/* xmlSecTransformExecuteMethod execute; */
    
    /* xmlSecTransform data/methods */
    NULL,
    xmlSecDigestTransformRead,		/* xmlSecTransformReadMethod readBin; */
    xmlSecDigestTransformWrite,		/* xmlSecTransformWriteMethod writeBin; */
    xmlSecDigestTransformFlush,		/* xmlSecTransformFlushMethod flushBin; */
    
    NULL,
    NULL,
        
    /* xmlSecDigestTransform data/methods */
    xmlSecDigestRipemd160Update,	/* xmlSecDigestUpdateMethod digestUpdate; */
    xmlSecDigestRipemd160Sign,		/* xmlSecDigestSignMethod digestSign; */
    xmlSecDigestRipemd160Verify		/* xmlSecDigestVerifyMethod digestVerify; */
};
xmlSecTransformId xmlSecDigestRipemd160 = (xmlSecTransformId)&xmlSecDigestRipemd160Id;  


#define XMLSEC_RIPEMD160_TRANSFORM_SIZE \
    (sizeof(xmlSecDigestTransform) + sizeof(RIPEMD160_CTX) +  RIPEMD160_DIGEST_LENGTH)
#define xmlSecDigestRipemd160Context(t) \
    ((RIPEMD160_CTX*)(((xmlSecDigestTransformPtr)( t ))->digestData))

/**
 * xmlSecDigestRipemd160Create:
 */
static xmlSecTransformPtr 
xmlSecDigestRipemd160Create(xmlSecTransformId id) {
    xmlSecDigestTransformPtr digest;

    xmlSecAssert2(id != NULL, NULL);
        
    if(id != xmlSecDigestRipemd160){
	xmlSecError(XMLSEC_ERRORS_HERE,
		    XMLSEC_ERRORS_R_INVALID_TRANSFORM,
		    "xmlSecDigestRipemd160");
	return(NULL);
    }

    /*
     * Allocate a new xmlSecTransform and fill the fields.
     */
    digest = (xmlSecDigestTransformPtr) xmlMalloc(XMLSEC_RIPEMD160_TRANSFORM_SIZE);
    if(digest == NULL) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    XMLSEC_ERRORS_R_MALLOC_FAILED,
		    "%d", XMLSEC_RIPEMD160_TRANSFORM_SIZE);
	return(NULL);
    }
    memset(digest, 0, XMLSEC_RIPEMD160_TRANSFORM_SIZE);
    
    digest->id = id;
    digest->digestData = ((unsigned char*)digest) + sizeof(xmlSecDigestTransform);
    digest->digest = ((unsigned char*)digest->digestData) + sizeof(RIPEMD160_CTX);
    digest->digestSize = RIPEMD160_DIGEST_LENGTH;

    RIPEMD160_Init(xmlSecDigestRipemd160Context(digest));    

    return((xmlSecTransformPtr)digest);
}

/**
 * xmlSecDigestRipemd160Destroy:
 */
static void 	
xmlSecDigestRipemd160Destroy(xmlSecTransformPtr transform) {
    xmlSecDigestTransformPtr digest;

    xmlSecAssert(transform != NULL);
        
    if(!xmlSecTransformCheckId(transform, xmlSecDigestRipemd160)) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    XMLSEC_ERRORS_R_INVALID_TRANSFORM,
		    "xmlSecDigestRipemd160");
	return;
    }    
    digest = (xmlSecDigestTransformPtr)transform;

    memset(digest, 0, XMLSEC_RIPEMD160_TRANSFORM_SIZE);
    xmlFree(digest);
}

/**
 * xmlSecDigestRipemd160Update:
 */
static int 	
xmlSecDigestRipemd160Update(xmlSecDigestTransformPtr transform,
			const unsigned char *buffer, size_t size) {
    xmlSecDigestTransformPtr digest;

    xmlSecAssert2(transform != NULL, -1);
    
    if(!xmlSecTransformCheckId(transform, xmlSecDigestRipemd160)) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    XMLSEC_ERRORS_R_INVALID_TRANSFORM,
		    "xmlSecDigestRipemd160");
	return(-1);
    }    
    digest = (xmlSecDigestTransformPtr)transform;
    
    if((buffer == NULL) || (size == 0) || (digest->status != xmlSecTransformStatusNone)) {
	/* nothing to update */
	return(0);
    }
    
    RIPEMD160_Update(xmlSecDigestRipemd160Context(digest), buffer, size);
    return(0);
}

/**
 * xmlSecDigestRipemd160Sign:
 */
static int 	
xmlSecDigestRipemd160Sign(xmlSecDigestTransformPtr transform,
			unsigned char **buffer, size_t *size) {
    xmlSecDigestTransformPtr digest;

    xmlSecAssert2(transform != NULL, -1);
    
    if(!xmlSecTransformCheckId(transform, xmlSecDigestRipemd160)) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    XMLSEC_ERRORS_R_INVALID_TRANSFORM,
		    "xmlSecDigestRipemd160");
	return(-1);
    }    
    digest = (xmlSecDigestTransformPtr)transform;
    if(digest->status != xmlSecTransformStatusNone) {
	return(0);
    }
    
    RIPEMD160_Final(digest->digest, xmlSecDigestRipemd160Context(digest));
    if(buffer != NULL) {
	(*buffer) = digest->digest;
    }        
    if(size != NULL) {
	(*size) = digest->digestSize;
    }        
    digest->status = xmlSecTransformStatusOk;
    return(0);
}

/**
 * xmlSecDigestRipemd160Verify:
 */
static int
xmlSecDigestRipemd160Verify(xmlSecDigestTransformPtr transform,
			 const unsigned char *buffer, size_t size) {
    xmlSecDigestTransformPtr digest;

    xmlSecAssert2(transform != NULL, -1);
    
    if(!xmlSecTransformCheckId(transform, xmlSecDigestRipemd160)) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    XMLSEC_ERRORS_R_INVALID_TRANSFORM,
		    "xmlSecDigestRipemd160");
	return(-1);
    }    
    digest = (xmlSecDigestTransformPtr)transform;

    if(digest->status != xmlSecTransformStatusNone) {
	return(0);
    }
    
    RIPEMD160_Final(digest->digest, xmlSecDigestRipemd160Context(digest));
    if((buffer == NULL) || (size != digest->digestSize) || (digest->digest == NULL)) {
	digest->status = xmlSecTransformStatusFail;
    } else if(memcmp(digest->digest, buffer, digest->digestSize) != 0){
	digest->status = xmlSecTransformStatusFail;
    } else {
	digest->status = xmlSecTransformStatusOk;
    }
    return(0);
}

#endif /* XMLSEC_NO_RIPEMD160 */
