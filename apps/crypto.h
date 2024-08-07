/**
 * XMLSec library
 *
 *
 * See Copyright for the status of this software.
 *
 * Copyright (C) 2002-2024 Aleksey Sanin <aleksey@aleksey.com>. All Rights Reserved.
 */
#ifndef __XMLSEC_APPS_CRYPTO_H__
#define __XMLSEC_APPS_CRYPTO_H__

#include <libxml/tree.h>
#include <xmlsec/xmlsec.h>
#include <xmlsec/keys.h>
#include <xmlsec/keyinfo.h>
#include <xmlsec/keysmngr.h>
#include <xmlsec/crypto.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int     xmlSecAppCryptoInit                                     (const char* config);
int     xmlSecAppCryptoShutdown                                 (void);

xmlSecKeyPtr xmlSecAppCryptoKeyGenerate                         (const char* keyKlassAndSize,
                                                                 const char* name,
                                                                 xmlSecKeyDataType type);

/*****************************************************************************
 *
 * Simple keys manager
 *
 ****************************************************************************/
int     xmlSecAppCryptoSimpleKeysMngrInit                       (xmlSecKeysMngrPtr mngr);
int     xmlSecAppCryptoSimpleKeysMngrLoad                       (xmlSecKeysMngrPtr mngr,
                                                                 const char* filename);
int     xmlSecAppCryptoSimpleKeysMngrSave                       (xmlSecKeysMngrPtr mngr,
                                                                 const char* filename,
                                                                 xmlSecKeyDataType type);
int     xmlSecAppCryptoSimpleKeysMngrCertLoad                   (xmlSecKeysMngrPtr mngr,
                                                                 const char* filename,
                                                                 xmlSecKeyDataFormat format,
                                                                 xmlSecKeyDataType type);
int     xmlSecAppCryptoSimpleKeysMngrCrlLoad                    (xmlSecKeysMngrPtr mngr,
                                                                 const char* filename,
                                                                 xmlSecKeyDataFormat format);
int     xmlSecAppCryptoSimpleKeysMngrKeyAndCertsLoad            (xmlSecKeysMngrPtr mngr,
                                                                 const char* files,
                                                                 const char* pwd,
                                                                 const char* name,
                                                                 xmlSecKeyDataType type,
                                                                 xmlSecKeyDataFormat format,
                                                                 xmlSecKeyInfoCtxPtr keyInfoCtx,
                                                                 int verifyKey);
int     xmlSecAppCryptoSimpleKeysMngrEngineKeyAndCertsLoad      (xmlSecKeysMngrPtr mngr,
                                                                 const char* engineAndKeyId,
                                                                 const char* certFiles,
                                                                 const char* pwd,
                                                                 const char* name,
                                                                 xmlSecKeyDataType type,
                                                                 xmlSecKeyDataFormat keyFormat,
                                                                 xmlSecKeyDataFormat certFormat,
                                                                 xmlSecKeyInfoCtxPtr keyInfoCtx,
                                                                 int verifyKey);
int     xmlSecAppCryptoSimpleKeysMngrPkcs12KeyLoad              (xmlSecKeysMngrPtr mngr,
                                                                 const char* filename,
                                                                 const char* pwd,
                                                                 const char* name,
                                                                 xmlSecKeyInfoCtxPtr keyInfoCtx,
                                                                 int verifyKey);
int     xmlSecAppCryptoSimpleKeysMngrBinaryKeyLoad              (xmlSecKeysMngrPtr mngr,
                                                                 const char* keyKlass,
                                                                 const char* filename,
                                                                 const char* name);
int     xmlSecAppCryptoSimpleKeysMngrKeyGenerate                (xmlSecKeysMngrPtr mngr,
                                                                 const char* keyKlassAndSize,
                                                                 const char* name);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __XMLSEC_APPS_CRYPTO_H__ */
