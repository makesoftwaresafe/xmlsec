/*
 * XML Security Library (http://www.aleksey.com/xmlsec).
 *
 *
 * This is free software; see Copyright file in the source
 * distribution for preciese wording.
 *
 * Copyright (C) 2002-2024 Aleksey Sanin <aleksey@aleksey.com>. All Rights Reserved.
 */
#ifndef __XMLSEC_APP_H__
#define __XMLSEC_APP_H__

#ifndef XMLSEC_NO_CRYPTO_DYNAMIC_LOADING

#if !defined(IN_XMLSEC) && !defined(XMLSEC_CRYPTO_DYNAMIC_LOADING)
#error To use dynamic crypto engines loading define XMLSEC_CRYPTO_DYNAMIC_LOADING
#endif /* !defined(IN_XMLSEC) && !defined(XMLSEC_CRYPTO_DYNAMIC_LOADING) */

#include <libxml/tree.h>
#include <libxml/xmlIO.h>

#include <xmlsec/exports.h>
#include <xmlsec/xmlsec.h>
#include <xmlsec/keysdata.h>
#include <xmlsec/keys.h>
#include <xmlsec/keysmngr.h>
#include <xmlsec/transforms.h>
#include <xmlsec/dl.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**********************************************************************
 *
 * Crypto Init/shutdown
 *
 *********************************************************************/
XMLSEC_EXPORT int                               xmlSecCryptoInit(void);
XMLSEC_EXPORT int                               xmlSecCryptoShutdown(void);
XMLSEC_EXPORT int                               xmlSecCryptoKeysMngrInit(xmlSecKeysMngrPtr mngr);

/*********************************************************************
 *
 * Key data ids
 *
 ********************************************************************/
/**
 * xmlSecKeyDataAesId:
 *
 * The AES key klass.
 */
#define xmlSecKeyDataAesId                      xmlSecKeyDataAesGetKlass()
XMLSEC_EXPORT xmlSecKeyDataId                   xmlSecKeyDataAesGetKlass(void);
/**
 * xmlSecKeyDataConcatKdfId:
 *
 * The ConcatKDF key klass.
 */
#define xmlSecKeyDataConcatKdfId                xmlSecKeyDataConcatKdfGetKlass()
XMLSEC_EXPORT xmlSecKeyDataId                   xmlSecKeyDataConcatKdfGetKlass(void);
/**
 * xmlSecKeyDataDesId:
 *
 * The DES key klass.
 */
#define xmlSecKeyDataDesId                      xmlSecKeyDataDesGetKlass()
XMLSEC_EXPORT xmlSecKeyDataId                   xmlSecKeyDataDesGetKlass(void);
/**
 * xmlSecKeyDataDhId:
 *
 * The DH key klass.
 */
#define xmlSecKeyDataDhId                       xmlSecKeyDataDhGetKlass()
XMLSEC_EXPORT xmlSecKeyDataId                   xmlSecKeyDataDhGetKlass(void);
/**
 * xmlSecKeyDataDsaId:
 *
 * The DSA key klass.
 */
#define xmlSecKeyDataDsaId                      xmlSecKeyDataDsaGetKlass()
XMLSEC_EXPORT xmlSecKeyDataId                   xmlSecKeyDataDsaGetKlass(void);
/**
 * xmlSecKeyDataEcId:
 *
 * The EC key klass.
 */
#define xmlSecKeyDataEcId                       xmlSecKeyDataEcGetKlass()
XMLSEC_EXPORT xmlSecKeyDataId                   xmlSecKeyDataEcGetKlass(void);
/**
 * xmlSecKeyDataGost2001Id:
 *
 * The GOST2001 key klass.
 */
#define xmlSecKeyDataGost2001Id                 xmlSecKeyDataGost2001GetKlass()
XMLSEC_EXPORT xmlSecKeyDataId                   xmlSecKeyDataGost2001GetKlass(void);
/**
 * xmlSecKeyDataGostR3410_2012_256Id:
 *
 * The GOST R 34.10-2012 256 bit key klass.
 */
#define xmlSecKeyDataGostR3410_2012_256Id       xmlSecKeyDataGostR3410_2012_256GetKlass()
XMLSEC_EXPORT xmlSecKeyDataId                   xmlSecKeyDataGostR3410_2012_256GetKlass(void);
/**
 * xmlSecKeyDataGostR3410_2012_512Id:
 *
 * The GOST R 34.10-2012 512 bit key klass.
 */
#define xmlSecKeyDataGostR3410_2012_512Id       xmlSecKeyDataGostR3410_2012_512GetKlass()
XMLSEC_EXPORT xmlSecKeyDataId                   xmlSecKeyDataGostR3410_2012_512GetKlass(void);
/**
 * xmlSecKeyDataHmacId:
 *
 * The HMAC key klass.
 */
#define xmlSecKeyDataHmacId                     xmlSecKeyDataHmacGetKlass()
XMLSEC_EXPORT xmlSecKeyDataId                   xmlSecKeyDataHmacGetKlass(void);
/**
 * xmlSecKeyDataPbkdf2Id:
 *
 * The PBKDF2 key klass.
 */
#define xmlSecKeyDataPbkdf2Id                   xmlSecKeyDataPbkdf2GetKlass()
XMLSEC_EXPORT xmlSecKeyDataId                   xmlSecKeyDataPbkdf2GetKlass(void);
/**
 * xmlSecKeyDataRsaId:
 *
 * The RSA key klass.
 */
#define xmlSecKeyDataRsaId                      xmlSecKeyDataRsaGetKlass()
XMLSEC_EXPORT xmlSecKeyDataId                   xmlSecKeyDataRsaGetKlass(void);
/**
 * xmlSecKeyDataX509Id:
 *
 * The X509 data klass.
 */
#define xmlSecKeyDataX509Id                     xmlSecKeyDataX509GetKlass()
XMLSEC_EXPORT xmlSecKeyDataId                   xmlSecKeyDataX509GetKlass(void);
/**
 * xmlSecKeyDataRawX509CertId:
 *
 * The  raw X509 certificate klass.
 */
#define xmlSecKeyDataRawX509CertId              xmlSecKeyDataRawX509CertGetKlass()
XMLSEC_EXPORT xmlSecKeyDataId                   xmlSecKeyDataRawX509CertGetKlass(void);
/**
 * xmlSecKeyDataDEREncodedKeyValueId:
 *
 * The DEREncodedKeyValue data klass.
 */
#define xmlSecKeyDataDEREncodedKeyValueId       xmlSecKeyDataDEREncodedKeyValueGetKlass()
XMLSEC_EXPORT xmlSecKeyDataId                   xmlSecKeyDataDEREncodedKeyValueGetKlass(void);

/*********************************************************************
 *
 * Key data store ids
 *
 ********************************************************************/
/**
 * xmlSecX509StoreId:
 *
 * The  X509 store klass.
 */
#define xmlSecX509StoreId                       xmlSecX509StoreGetKlass()
XMLSEC_EXPORT xmlSecKeyDataStoreId              xmlSecX509StoreGetKlass(void);

/********************************************************************
 *
 * Crypto transforms ids
 *
 * https://www.aleksey.com/xmlsec/xmldsig.html
 * https://www.aleksey.com/xmlsec/xmlenc.html
 *
 ********************************************************************/
/**
 * xmlSecTransformAes128CbcId:
 *
 * The AES128 CBC cipher transform klass.
 */
#define xmlSecTransformAes128CbcId              xmlSecTransformAes128CbcGetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformAes128CbcGetKlass(void);
/**
 * xmlSecTransformAes192CbcId:
 *
 * The AES192 CBC cipher transform klass.
 */
#define xmlSecTransformAes192CbcId              xmlSecTransformAes192CbcGetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformAes192CbcGetKlass(void);
/**
 * xmlSecTransformAes256CbcId:
 *
 * The AES256 CBC cipher transform klass.
 */
#define xmlSecTransformAes256CbcId              xmlSecTransformAes256CbcGetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformAes256CbcGetKlass(void);
/**
* xmlSecTransformAes128GcmId:
*
* The AES128 GCM cipher transform klass.
*/
#define xmlSecTransformAes128GcmId              xmlSecTransformAes128GcmGetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformAes128GcmGetKlass(void);
/**
* xmlSecTransformAes192GcmId:
*
* The AES192 GCM cipher transform klass.
*/
#define xmlSecTransformAes192GcmId              xmlSecTransformAes192GcmGetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformAes192GcmGetKlass(void);
/**
* xmlSecTransformAes256GcmId:
*
* The AES256 GCM cipher transform klass.
*/
#define xmlSecTransformAes256GcmId              xmlSecTransformAes256GcmGetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformAes256GcmGetKlass(void);
/**
 * xmlSecTransformKWAes128Id:
 *
 * The AES 128 key wrap transform klass.
 */
#define xmlSecTransformKWAes128Id               xmlSecTransformKWAes128GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformKWAes128GetKlass(void);
/**
 * xmlSecTransformKWAes192Id:
 *
 * The AES 192 key wrap transform klass.
 */
#define xmlSecTransformKWAes192Id               xmlSecTransformKWAes192GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformKWAes192GetKlass(void);
/**
 * xmlSecTransformKWAes256Id:
 *
 * The AES 256 key wrap transform klass.
 */
#define xmlSecTransformKWAes256Id               xmlSecTransformKWAes256GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformKWAes256GetKlass(void);
/**
* xmlSecTransformConcatKdfId:
*
* The ConcatKDF key derivation transform klass.
*/
#define xmlSecTransformConcatKdfId              xmlSecTransformConcatKdfGetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformConcatKdfGetKlass(void);
/**
 * xmlSecTransformDes3CbcId:
 *
 * The Triple DES encryption transform klass.
 */
#define xmlSecTransformDes3CbcId                xmlSecTransformDes3CbcGetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformDes3CbcGetKlass(void);
/**
 * xmlSecTransformKWDes3Id:
 *
 * The DES3 CBC cipher transform klass.
 */
#define xmlSecTransformKWDes3Id                 xmlSecTransformKWDes3GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformKWDes3GetKlass(void);
/**
* xmlSecTransformDhEsId:
*
* The DH-ES key agreement transform klass.
*/
#define xmlSecTransformDhEsId                   xmlSecTransformDhEsGetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformDhEsGetKlass(void);
/**
 * xmlSecTransformDsaSha1Id:
 *
 * The DSA-SHA1 signature transform klass.
 */
#define xmlSecTransformDsaSha1Id                xmlSecTransformDsaSha1GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformDsaSha1GetKlass(void);
/**
 * xmlSecTransformDsaSha256Id:
 *
 * The DSA-SHA2-256 signature transform klass.
 */
#define xmlSecTransformDsaSha256Id              xmlSecTransformDsaSha256GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformDsaSha256GetKlass(void);
/**
* xmlSecTransformEcdhId:
*
* The ECDH key agreement transform klass.
*/
#define xmlSecTransformEcdhId                   xmlSecTransformEcdhGetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformEcdhGetKlass(void);
/**
 * xmlSecTransformEcdsaSha1Id:
 *
 * The ECDSA-SHA1 signature transform klass.
 */
#define xmlSecTransformEcdsaSha1Id              xmlSecTransformEcdsaSha1GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformEcdsaSha1GetKlass(void);

/**
 * xmlSecTransformEcdsaSha224Id:
 *
 * The ECDSA-SHA2-224 signature transform klass.
 */
#define xmlSecTransformEcdsaSha224Id            xmlSecTransformEcdsaSha224GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformEcdsaSha224GetKlass(void);
/**
 * xmlSecTransformEcdsaSha256Id:
 *
 * The ECDSA-SHA2-256 signature transform klass.
 */
#define xmlSecTransformEcdsaSha256Id            xmlSecTransformEcdsaSha256GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformEcdsaSha256GetKlass(void);
/**
 * xmlSecTransformEcdsaSha384Id:
 *
 * The ECDS-SHA2-384 signature transform klass.
 */
#define xmlSecTransformEcdsaSha384Id            xmlSecTransformEcdsaSha384GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformEcdsaSha384GetKlass(void);
/**
 * xmlSecTransformEcdsaSha512Id:
 *
 * The ECDSA-SHA2-512 signature transform klass.
 */
#define xmlSecTransformEcdsaSha512Id            xmlSecTransformEcdsaSha512GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformEcdsaSha512GetKlass(void);

/**
 * xmlSecTransformEcdsaSha3_224Id:
 *
 * The ECDSA-SHA3-224 signature transform klass.
 */
#define xmlSecTransformEcdsaSha3_224Id          xmlSecTransformEcdsaSha3_224GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformEcdsaSha3_224GetKlass(void);
/**
 * xmlSecTransformEcdsaSha3_256Id:
 *
 * The ECDSA-SHA3-256 signature transform klass.
 */
#define xmlSecTransformEcdsaSha3_256Id          xmlSecTransformEcdsaSha3_256GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformEcdsaSha3_256GetKlass(void);
/**
 * xmlSecTransformEcdsaSha3_384Id:
 *
 * The ECDS-SHA3-384 signature transform klass.
 */
#define xmlSecTransformEcdsaSha3_384Id          xmlSecTransformEcdsaSha3_384GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformEcdsaSha3_384GetKlass(void);
/**
 * xmlSecTransformEcdsaSha3_512Id:
 *
 * The ECDSA-SHA3-512 signature transform klass.
 */
#define xmlSecTransformEcdsaSha3_512Id          xmlSecTransformEcdsaSha3_512GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformEcdsaSha3_512GetKlass(void);

/**
 * xmlSecTransformEcdsaRipemd160Id:
 *
 * The ECDSA-RIPEMD160 signature transform klass.
 */
#define xmlSecTransformEcdsaRipemd160Id         xmlSecTransformEcdsaRipemd160GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformEcdsaRipemd160GetKlass(void);
/**
 * xmlSecTransformGost2001GostR3411_94Id:
 *
 * The GOST2001-GOSTR3411_94 signature transform klass.
 */
#define xmlSecTransformGost2001GostR3411_94Id           xmlSecTransformGost2001GostR3411_94GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformGost2001GostR3411_94GetKlass(void);

/**
 * xmlSecTransformGostR3410_2012GostR3411_2012_256Id:
 *
 * The GOST R 34.10-2012 - GOST R 34.11-2012 256 bit signature transform klass.
 */
#define xmlSecTransformGostR3410_2012GostR3411_2012_256Id   xmlSecTransformGostR3410_2012GostR3411_2012_256GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformGostR3410_2012GostR3411_2012_256GetKlass(void);

/**
 * xmlSecTransformGostR3410_2012GostR3411_2012_512Id:
 *
 * The GOST R 34.10-2012 - GOST R 34.11-2012 512 bit signature transform klass.
 */
#define xmlSecTransformGostR3410_2012GostR3411_2012_512Id   xmlSecTransformGostR3410_2012GostR3411_2012_512GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformGostR3410_2012GostR3411_2012_512GetKlass(void);

/**
 * xmlSecTransformHmacMd5Id:
 *
 * The HMAC with MD5 signature transform klass.
 */
#define xmlSecTransformHmacMd5Id                xmlSecTransformHmacMd5GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformHmacMd5GetKlass(void);
/**
 * xmlSecTransformHmacRipemd160Id:
 *
 * The HMAC with RipeMD160 signature transform klass.
 */
#define xmlSecTransformHmacRipemd160Id          xmlSecTransformHmacRipemd160GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformHmacRipemd160GetKlass(void);
/**
 * xmlSecTransformHmacSha1Id:
 *
 * The HMAC with SHA1 signature transform klass.
 */
#define xmlSecTransformHmacSha1Id               xmlSecTransformHmacSha1GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformHmacSha1GetKlass(void);
/**
 * xmlSecTransformHmacSha224Id:
 *
 * The HMAC with SHA2-224 signature transform klass.
 */
#define xmlSecTransformHmacSha224Id             xmlSecTransformHmacSha224GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformHmacSha224GetKlass(void);
/**
 * xmlSecTransformHmacSha256Id:
 *
 * The HMAC with SHA2-256 signature transform klass.
 */
#define xmlSecTransformHmacSha256Id             xmlSecTransformHmacSha256GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformHmacSha256GetKlass(void);
/**
 * xmlSecTransformHmacSha384Id:
 *
 * The HMAC with SHA2-384 signature transform klass.
 */
#define xmlSecTransformHmacSha384Id             xmlSecTransformHmacSha384GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformHmacSha384GetKlass(void);
/**
 * xmlSecTransformHmacSha512Id:
 *
 * The HMAC with SHA2-512 signature transform klass.
 */
#define xmlSecTransformHmacSha512Id             xmlSecTransformHmacSha512GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformHmacSha512GetKlass(void);
/**
 * xmlSecTransformMd5Id:
 *
 * The MD5 digest transform klass.
 */
#define xmlSecTransformMd5Id                    xmlSecTransformMd5GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformMd5GetKlass(void);
/**
* xmlSecTransformPbkdf2Id:
*
* The PBKDF2 key derivation transform klass.
*/
#define xmlSecTransformPbkdf2Id                 xmlSecTransformPbkdf2GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformPbkdf2GetKlass(void);
/**
 * xmlSecTransformRipemd160Id:
 *
 * The RIPEMD160 digest transform klass.
 */
#define xmlSecTransformRipemd160Id              xmlSecTransformRipemd160GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformRipemd160GetKlass(void);
/**
 * xmlSecTransformRsaMd5Id:
 *
 * The RSA-MD5 signature transform klass.
 */
#define xmlSecTransformRsaMd5Id                 xmlSecTransformRsaMd5GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformRsaMd5GetKlass(void);
/**
 * xmlSecTransformRsaRipemd160Id:
 *
 * The RSA-RIPEMD160 signature transform klass.
 */
#define xmlSecTransformRsaRipemd160Id           xmlSecTransformRsaRipemd160GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformRsaRipemd160GetKlass(void);
/**
 * xmlSecTransformRsaSha1Id:
 *
 * The RSA-SHA1 signature transform klass.
 */
#define xmlSecTransformRsaSha1Id                xmlSecTransformRsaSha1GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformRsaSha1GetKlass(void);
/**
 * xmlSecTransformRsaSha224Id:
 *
 * The RSA-SHA2-224 signature transform klass.
 */
#define xmlSecTransformRsaSha224Id              xmlSecTransformRsaSha224GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformRsaSha224GetKlass(void);
/**
 * xmlSecTransformRsaSha256Id:
 *
 * The RSA-SHA2-256 signature transform klass.
 */
#define xmlSecTransformRsaSha256Id              xmlSecTransformRsaSha256GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformRsaSha256GetKlass(void);
/**
 * xmlSecTransformRsaSha384Id:
 *
 * The RSA-SHA2-384 signature transform klass.
 */
#define xmlSecTransformRsaSha384Id              xmlSecTransformRsaSha384GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformRsaSha384GetKlass(void);
/**
 * xmlSecTransformRsaSha512Id:
 *
 * The RSA-SHA2-512 signature transform klass.
 */
#define xmlSecTransformRsaSha512Id              xmlSecTransformRsaSha512GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformRsaSha512GetKlass(void);

/**
 * xmlSecTransformRsaPssSha1Id:
 *
 * The RSA-PSS-SHA1 signature transform klass.
 */
#define xmlSecTransformRsaPssSha1Id             xmlSecTransformRsaPssSha1GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformRsaPssSha1GetKlass(void);

/**
 * xmlSecTransformRsaPssSha224Id:
 *
 * The RSA-PSS-SHA2-224 signature transform klass.
 */
#define xmlSecTransformRsaPssSha224Id           xmlSecTransformRsaPssSha224GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformRsaPssSha224GetKlass(void);
/**
 * xmlSecTransformRsaPssSha256Id:
 *
 * The RSA-PSS-SHA2-256 signature transform klass.
 */
#define xmlSecTransformRsaPssSha256Id           xmlSecTransformRsaPssSha256GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformRsaPssSha256GetKlass(void);
/**
 * xmlSecTransformRsaPssSha384Id:
 *
 * The RSA-PSS-SHA2-384 signature transform klass.
 */
#define xmlSecTransformRsaPssSha384Id           xmlSecTransformRsaPssSha384GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformRsaPssSha384GetKlass(void);
/**
 * xmlSecTransformRsaPssSha512Id:
 *
 * The RSA-PSS-SHA2-512 signature transform klass.
 */
#define xmlSecTransformRsaPssSha512Id           xmlSecTransformRsaPssSha512GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformRsaPssSha512GetKlass(void);

/**
 * xmlSecTransformRsaPssSha3_224Id:
 *
 * The RSA-PSS-SHA3-224 signature transform klass.
 */
#define xmlSecTransformRsaPssSha3_224Id         xmlSecTransformRsaPssSha3_224GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformRsaPssSha3_224GetKlass(void);
/**
 * xmlSecTransformRsaPssSha3_256Id:
 *
 * The RSA-PSS-SHA3-256 signature transform klass.
 */
#define xmlSecTransformRsaPssSha3_256Id         xmlSecTransformRsaPssSha3_256GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformRsaPssSha3_256GetKlass(void);
/**
 * xmlSecTransformRsaPssSha3_384Id:
 *
 * The RSA-PSS-SHA3-384 signature transform klass.
 */
#define xmlSecTransformRsaPssSha3_384Id         xmlSecTransformRsaPssSha3_384GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformRsaPssSha3_384GetKlass(void);
/**
 * xmlSecTransformRsaPssSha3_512Id:
 *
 * The RSA-PSS-SHA3-512 signature transform klass.
 */
#define xmlSecTransformRsaPssSha3_512Id         xmlSecTransformRsaPssSha3_512GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformRsaPssSha3_512GetKlass(void);

/**
 * xmlSecTransformRsaPkcs1Id:
 *
 * The RSA PKCS1 key transport transform klass.
 */
#define xmlSecTransformRsaPkcs1Id               xmlSecTransformRsaPkcs1GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformRsaPkcs1GetKlass(void);
/**
 * xmlSecTransformRsaOaepId:
 *
 * The RSA PKCS1 key transport transform klass (XMLEnc 1.0).
 */
#define xmlSecTransformRsaOaepId                xmlSecTransformRsaOaepGetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformRsaOaepGetKlass(void);
/**
 * xmlSecTransformRsaOaepEnc11Id:
 *
 * The RSA PKCS1 key transport transform klass (XMLEnc 1.1).
 */
#define xmlSecTransformRsaOaepEnc11Id           xmlSecTransformRsaOaepEnc11GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformRsaOaepEnc11GetKlass(void);
/**
 * xmlSecTransformGostR3411_94Id:
 *
 * The GOSTR3411_94 digest transform klass.
 */
#define xmlSecTransformGostR3411_94Id           xmlSecTransformGostR3411_94GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformGostR3411_94GetKlass(void);
/**
 * xmlSecTransformGostR3411_2012_256Id:
 *
 * The GOST R 34.11-2012 256 bit digest transform klass.
 */
#define xmlSecTransformGostR3411_2012_256Id     xmlSecTransformGostR3411_2012_256GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformGostR3411_2012_256GetKlass(void);
/**
 * xmlSecTransformGostR3411_2012_512Id:
 *
 * The GOST R 34.11-2012 512 bit digest transform klass.
 */
#define xmlSecTransformGostR3411_2012_512Id     xmlSecTransformGostR3411_2012_512GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformGostR3411_2012_512GetKlass(void);

/**
 * xmlSecTransformSha1Id:
 *
 * The SHA1 digest transform klass.
 */
#define xmlSecTransformSha1Id                   xmlSecTransformSha1GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformSha1GetKlass(void);

/**
 * xmlSecTransformSha224Id:
 *
 * The SHA2-224 digest transform klass.
 */
#define xmlSecTransformSha224Id                 xmlSecTransformSha224GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformSha224GetKlass(void);
/**
 * xmlSecTransformSha256Id:
 *
 * The SHA2-256 digest transform klass.
 */
#define xmlSecTransformSha256Id                 xmlSecTransformSha256GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformSha256GetKlass(void);
/**
 * xmlSecTransformSha384Id:
 *
 * The SHA2-384 digest transform klass.
 */
#define xmlSecTransformSha384Id                 xmlSecTransformSha384GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformSha384GetKlass(void);
/**
 * xmlSecTransformSha512Id:
 *
 * The SHA2-512 digest transform klass.
 */
#define xmlSecTransformSha512Id                 xmlSecTransformSha512GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformSha512GetKlass(void);


/**
 * xmlSecTransformSha3_224Id:
 *
 * The SHA3-224 digest transform klass.
 */
#define xmlSecTransformSha3_224Id               xmlSecTransformSha3_224GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformSha3_224GetKlass(void);
/**
 * xmlSecTransformSha3_256Id:
 *
 * The SHA3-256 digest transform klass.
 */
#define xmlSecTransformSha3_256Id               xmlSecTransformSha3_256GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformSha3_256GetKlass(void);
/**
 * xmlSecTransformSha3_384Id:
 *
 * The SHA3-384 digest transform klass.
 */
#define xmlSecTransformSha3_384Id               xmlSecTransformSha3_384GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformSha3_384GetKlass(void);
/**
 * xmlSecTransformSha3_512Id:
 *
 * The SHA3-512 digest transform klass.
 */
#define xmlSecTransformSha3_512Id               xmlSecTransformSha3_512GetKlass()
XMLSEC_EXPORT xmlSecTransformId                 xmlSecTransformSha3_512GetKlass(void);

/*********************************************************************
 *
 * High level routines form xmlsec command line utility
 *
 ********************************************************************/
XMLSEC_EXPORT int                               xmlSecCryptoAppInit             (const char* config);
XMLSEC_EXPORT int                               xmlSecCryptoAppShutdown         (void);
XMLSEC_EXPORT int                               xmlSecCryptoAppDefaultKeysMngrInit      (xmlSecKeysMngrPtr mngr);
XMLSEC_EXPORT int                               xmlSecCryptoAppDefaultKeysMngrAdoptKey  (xmlSecKeysMngrPtr mngr,
                                                                                         xmlSecKeyPtr key);
XMLSEC_EXPORT int                               xmlSecCryptoAppDefaultKeysMngrVerifyKey (xmlSecKeysMngrPtr mngr,
                                                                                         xmlSecKeyPtr key,
                                                                                         xmlSecKeyInfoCtxPtr keyInfoCtx);
XMLSEC_EXPORT int                               xmlSecCryptoAppDefaultKeysMngrLoad      (xmlSecKeysMngrPtr mngr,
                                                                                         const char* uri);
XMLSEC_EXPORT int                               xmlSecCryptoAppDefaultKeysMngrSave      (xmlSecKeysMngrPtr mngr,
                                                                                         const char* filename,
                                                                                         xmlSecKeyDataType type);
XMLSEC_EXPORT int                               xmlSecCryptoAppKeysMngrCertLoad (xmlSecKeysMngrPtr mngr,
                                                                                 const char *filename,
                                                                                 xmlSecKeyDataFormat format,
                                                                                 xmlSecKeyDataType type);
XMLSEC_EXPORT int                               xmlSecCryptoAppKeysMngrCertLoadMemory(xmlSecKeysMngrPtr mngr,
                                                                                 const xmlSecByte* data,
                                                                                 xmlSecSize dataSize,
                                                                                 xmlSecKeyDataFormat format,
                                                                                 xmlSecKeyDataType type);
XMLSEC_EXPORT int                               xmlSecCryptoAppKeysMngrCrlLoad  (xmlSecKeysMngrPtr mngr,
                                                                                 const char *filename,
                                                                                 xmlSecKeyDataFormat format);
XMLSEC_EXPORT int                               xmlSecCryptoAppKeysMngrCrlLoadMemory(xmlSecKeysMngrPtr mngr,
                                                                                 const xmlSecByte* data,
                                                                                 xmlSecSize dataSize,
                                                                                 xmlSecKeyDataFormat format);
XMLSEC_EXPORT xmlSecKeyPtr                      xmlSecCryptoAppKeyLoadEx        (const char *filename,
                                                                                 xmlSecKeyDataType type,
                                                                                 xmlSecKeyDataFormat format,
                                                                                 const char *pwd,
                                                                                 void* pwdCallback,
                                                                                 void* pwdCallbackCtx);
XMLSEC_EXPORT xmlSecKeyPtr                      xmlSecCryptoAppKeyLoadMemory    (const xmlSecByte* data,
                                                                                 xmlSecSize dataSize,
                                                                                 xmlSecKeyDataFormat format,
                                                                                 const char *pwd,
                                                                                 void* pwdCallback,
                                                                                 void* pwdCallbackCtx);
XMLSEC_EXPORT xmlSecKeyPtr                      xmlSecCryptoAppPkcs12Load       (const char* filename,
                                                                                 const char* pwd,
                                                                                 void* pwdCallback,
                                                                                 void* pwdCallbackCtx);
XMLSEC_EXPORT xmlSecKeyPtr                      xmlSecCryptoAppPkcs12LoadMemory (const xmlSecByte* data,
                                                                                 xmlSecSize dataSize,
                                                                                 const char *pwd,
                                                                                 void* pwdCallback,
                                                                                 void* pwdCallbackCtx);
XMLSEC_EXPORT int                               xmlSecCryptoAppKeyCertLoad      (xmlSecKeyPtr key,
                                                                                 const char* filename,
                                                                                 xmlSecKeyDataFormat format);
XMLSEC_EXPORT int                               xmlSecCryptoAppKeyCertLoadMemory(xmlSecKeyPtr key,
                                                                                 const xmlSecByte* data,
                                                                                 xmlSecSize dataSize,
                                                                                 xmlSecKeyDataFormat format);
XMLSEC_EXPORT void*                             xmlSecCryptoAppGetDefaultPwdCallback(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XMLSEC_NO_CRYPTO_DYNAMIC_LOADING */

#endif /* __XMLSEC_APP_H__ */
