//
//  BREthereumEWMPersist.c
//  BRCore
//
//  Created by Ed Gamble on 9/24/19.
//  Copyright © 2019 Breadwinner AG. All rights reserved.
//
//
//  See the LICENSE file at the project root for license information.
//  See the CONTRIBUTORS file at the project root for a list of contributors.

#include "BREthereumEWMPrivate.h"

/// MARK: - Transaction File Service
#define fileServiceTypeTransactions "transactions"

enum {
    EWM_TRANSACTION_VERSION_1
};

static UInt256
fileServiceTypeTransactionV1Identifier (BRFileServiceContext context,
                                        BRFileService fs,
                                        const void *entity) {
    BREthereumTransaction transaction = (BREthereumTransaction) entity;
    BREthereumHash hash = transactionGetHash(transaction);

    UInt256 result;
    memcpy (result.u8, hash.bytes, ETHEREUM_HASH_BYTES);
    return result;
}

static uint8_t *
fileServiceTypeTransactionV1Writer (BRFileServiceContext context,
                                    BRFileService fs,
                                    const void* entity,
                                    uint32_t *bytesCount) {
    BREthereumEWM ewm = context;
    BREthereumTransaction transaction = (BREthereumTransaction) entity;

    BRRlpItem item = transactionRlpEncode(transaction, ewm->network, RLP_TYPE_ARCHIVE, ewm->coder);
    BRRlpData data = rlpItemGetData (ewm->coder, item);
    rlpItemRelease (ewm->coder, item);

    *bytesCount = (uint32_t) data.bytesCount;
    return data.bytes;
}

static void *
fileServiceTypeTransactionV1Reader (BRFileServiceContext context,
                                    BRFileService fs,
                                    uint8_t *bytes,
                                    uint32_t bytesCount) {
    BREthereumEWM ewm = context;

    BRRlpData data = { bytesCount, bytes };
    BRRlpItem item = rlpDataGetItem (ewm->coder, data);

    BREthereumTransaction transaction = transactionRlpDecode(item, ewm->network, RLP_TYPE_ARCHIVE, ewm->coder);
    rlpItemRelease (ewm->coder, item);

    return transaction;
}

/// MARK: - Log File Service

#define fileServiceTypeLogs "logs"

enum {
    EWM_LOG_VERSION_1
};

static UInt256
fileServiceTypeLogV1Identifier (BRFileServiceContext context,
                                BRFileService fs,
                                const void *entity) {
    const BREthereumLog log = (BREthereumLog) entity;
    BREthereumHash hash = logGetHash( log);

    UInt256 result;
    memcpy (result.u8, hash.bytes, ETHEREUM_HASH_BYTES);
    return result;
}

static uint8_t *
fileServiceTypeLogV1Writer (BRFileServiceContext context,
                            BRFileService fs,
                            const void* entity,
                            uint32_t *bytesCount) {
    BREthereumEWM ewm = context;
    BREthereumLog log = (BREthereumLog) entity;

    BRRlpItem item = logRlpEncode (log, RLP_TYPE_ARCHIVE, ewm->coder);
    BRRlpData data = rlpItemGetData (ewm->coder, item);
    rlpItemRelease (ewm->coder, item);

    *bytesCount = (uint32_t) data.bytesCount;
    return data.bytes;
}

static void *
fileServiceTypeLogV1Reader (BRFileServiceContext context,
                            BRFileService fs,
                            uint8_t *bytes,
                            uint32_t bytesCount) {
    BREthereumEWM ewm = context;

    BRRlpData data = { bytesCount, bytes };
    BRRlpItem item = rlpDataGetItem (ewm->coder, data);

    BREthereumLog log = logRlpDecode(item, RLP_TYPE_ARCHIVE, ewm->coder);
    rlpItemRelease (ewm->coder, item);

    return log;
}

/// MARK: - Exchange Service

#define fileServiceTypeExchanges "exchanges"

enum {
    EWM_EXCHANGE_VERSION_1
};

static UInt256
fileServiceTypeExchangeV1Identifier (BRFileServiceContext context,
                                     BRFileService fs,
                                     const void *entity) {
    const BREthereumExchange exchange = (BREthereumExchange) entity;
    BREthereumHash hash = ethExchangeGetHash(exchange);

    UInt256 result;
    memcpy (result.u8, hash.bytes, ETHEREUM_HASH_BYTES);
    return result;
}

static uint8_t *
fileServiceTypeExchangeV1Writer (BRFileServiceContext context,
                                 BRFileService fs,
                                 const void* entity,
                                 uint32_t *bytesCount) {
    BREthereumEWM ewm = context;
    BREthereumExchange exchange = (BREthereumExchange) entity;

    BRRlpItem item = ethExchangeRlpEncode (exchange, RLP_TYPE_ARCHIVE, ewm->coder);
    BRRlpData data = rlpItemGetData (ewm->coder, item);
    rlpItemRelease (ewm->coder, item);

    *bytesCount = (uint32_t) data.bytesCount;
    return data.bytes;
}

static void *
fileServiceTypeExchangeV1Reader (BRFileServiceContext context,
                                 BRFileService fs,
                                 uint8_t *bytes,
                                 uint32_t bytesCount) {
    BREthereumEWM ewm = context;

    BRRlpData data = { bytesCount, bytes };
    BRRlpItem item = rlpDataGetItem (ewm->coder, data);

    BREthereumExchange exchange = ethExchangeRlpDecode (item, RLP_TYPE_ARCHIVE, ewm->coder);
    rlpItemRelease (ewm->coder, item);

    return exchange;
}

/// MARK: - Block File Service

#define fileServiceTypeBlocks "blocks"
enum {
    EWM_BLOCK_VERSION_1
};

static UInt256
fileServiceTypeBlockV1Identifier (BRFileServiceContext context,
                                  BRFileService fs,
                                  const void *entity) {
    const BREthereumBlock block = (BREthereumBlock) entity;
    BREthereumHash hash = blockGetHash(block);

    UInt256 result;
    memcpy (result.u8, hash.bytes, ETHEREUM_HASH_BYTES);
    return result;
}

static uint8_t *
fileServiceTypeBlockV1Writer (BRFileServiceContext context,
                              BRFileService fs,
                              const void* entity,
                              uint32_t *bytesCount) {
    BREthereumEWM ewm = context;
    BREthereumBlock block = (BREthereumBlock) entity;

    BRRlpItem item = blockRlpEncode(block, ewm->network, RLP_TYPE_ARCHIVE, ewm->coder);
    BRRlpData data = rlpItemGetData (ewm->coder, item);
    rlpItemRelease (ewm->coder, item);

    *bytesCount = (uint32_t) data.bytesCount;
    return data.bytes;
}

static void *
fileServiceTypeBlockV1Reader (BRFileServiceContext context,
                              BRFileService fs,
                              uint8_t *bytes,
                              uint32_t bytesCount) {
    BREthereumEWM ewm = context;

    BRRlpData data = { bytesCount, bytes };
    BRRlpItem item = rlpDataGetItem (ewm->coder, data);

    BREthereumBlock block = blockRlpDecode (item, ewm->network, RLP_TYPE_ARCHIVE, ewm->coder);
    rlpItemRelease (ewm->coder, item);

    return block;
}

// MARK: - Node File Service

#define fileServiceTypeNodes "nodes"
enum {
    EWM_NODE_VERSION_1
};

static UInt256
fileServiceTypeNodeV1Identifier (BRFileServiceContext context,
                                 BRFileService fs,
                                 const void *entity) {
    const BREthereumNodeConfig node = (BREthereumNodeConfig) entity;

    BREthereumHash hash = nodeConfigGetHash(node);

    UInt256 result;
    memcpy (result.u8, hash.bytes, ETHEREUM_HASH_BYTES);
    return result;
}

static uint8_t *
fileServiceTypeNodeV1Writer (BRFileServiceContext context,
                             BRFileService fs,
                             const void* entity,
                             uint32_t *bytesCount) {
    BREthereumEWM ewm = context;
    const BREthereumNodeConfig node = (BREthereumNodeConfig) entity;

    BRRlpItem item = nodeConfigEncode (node, ewm->coder);
    BRRlpData data = rlpItemGetData (ewm->coder, item);
    rlpItemRelease (ewm->coder, item);

    *bytesCount = (uint32_t) data.bytesCount;
    return data.bytes;
}

static void *
fileServiceTypeNodeV1Reader (BRFileServiceContext context,
                             BRFileService fs,
                             uint8_t *bytes,
                             uint32_t bytesCount) {
    BREthereumEWM ewm = context;

    BRRlpData data = { bytesCount, bytes };
    BRRlpItem item = rlpDataGetItem (ewm->coder, data);

    BREthereumNodeConfig node = nodeConfigDecode (item, ewm->coder);
    rlpItemRelease (ewm->coder, item);

    return node;
}

/// MARK: - Token File Service

#define fileServiceTypeTokens "tokens"

enum {
    EWM_TOKEN_VERSION_1
};

static UInt256
fileServiceTypeTokenV1Identifier (BRFileServiceContext context,
                                        BRFileService fs,
                                        const void *entity) {
    BREthereumToken token = (BREthereumToken) entity;
    BREthereumHash hash = ethTokenGetHash(token);

    UInt256 result;
    memcpy (result.u8, hash.bytes, ETHEREUM_HASH_BYTES);
    return result;
}

static uint8_t *
fileServiceTypeTokenV1Writer (BRFileServiceContext context,
                                    BRFileService fs,
                                    const void* entity,
                                    uint32_t *bytesCount) {
    BREthereumEWM ewm = context;
    BREthereumToken token = (BREthereumToken) entity;

    BRRlpItem item = ethTokenRlpEncode(token, ewm->coder);
    BRRlpData data = rlpItemGetData (ewm->coder, item);
    rlpItemRelease (ewm->coder, item);

    *bytesCount = (uint32_t) data.bytesCount;
    return data.bytes;
}

static void *
fileServiceTypeTokenV1Reader (BRFileServiceContext context,
                                    BRFileService fs,
                                    uint8_t *bytes,
                                    uint32_t bytesCount) {
    BREthereumEWM ewm = context;

    BRRlpData data = { bytesCount, bytes };
    BRRlpItem item = rlpDataGetItem (ewm->coder, data);

    BREthereumToken token = ethTokenRlpDecode(item, ewm->coder);
    rlpItemRelease (ewm->coder, item);

    return token;
}

/// MARK: - Wallet File Service

#define fileServiceTypeWallets "wallets"

enum {
    EWM_WALLET_VERSION_1
};

static UInt256
fileServiceTypeWalletV1Identifier (BRFileServiceContext context,
                                   BRFileService fs,
                                   const void *entity) {
    BREthereumWalletState state = (BREthereumWalletState) entity;
    BREthereumHash hash = walletStateGetHash (state);

    UInt256 result;
    memcpy (result.u8, hash.bytes, ETHEREUM_HASH_BYTES);
    return result;
}

static uint8_t *
fileServiceTypeWalletV1Writer (BRFileServiceContext context,
                               BRFileService fs,
                               const void* entity,
                               uint32_t *bytesCount) {
    BREthereumEWM ewm = context;
    BREthereumWalletState state = (BREthereumWalletState) entity;

    BRRlpItem item = walletStateEncode (state, ewm->coder);
    BRRlpData data = rlpItemGetData (ewm->coder, item);
    rlpItemRelease (ewm->coder, item);

    *bytesCount = (uint32_t) data.bytesCount;
    return data.bytes;
}

static void *
fileServiceTypeWalletV1Reader (BRFileServiceContext context,
                               BRFileService fs,
                               uint8_t *bytes,
                               uint32_t bytesCount) {
    BREthereumEWM ewm = context;

    BRRlpData data = { bytesCount, bytes };
    BRRlpItem item = rlpDataGetItem (ewm->coder, data);

    BREthereumWalletState state = walletStateDecode(item, ewm->coder);
    rlpItemRelease (ewm->coder, item);

    return state;
}

static BRFileServiceTypeSpecification fileServiceSpecifications[] = {
    {
        fileServiceTypeTransactions,
        EWM_TRANSACTION_VERSION_1,
        1,
        {
            {
                EWM_TRANSACTION_VERSION_1,
                fileServiceTypeTransactionV1Identifier,
                fileServiceTypeTransactionV1Reader,
                fileServiceTypeTransactionV1Writer
            }
        }
    },

    {
        fileServiceTypeLogs,
        EWM_LOG_VERSION_1,
        1,
        {
            {
                EWM_LOG_VERSION_1,
                fileServiceTypeLogV1Identifier,
                fileServiceTypeLogV1Reader,
                fileServiceTypeLogV1Writer
            }
        }
    },

    {
        fileServiceTypeExchanges,
        EWM_EXCHANGE_VERSION_1,
        1,
        {
            {
                EWM_EXCHANGE_VERSION_1,
                fileServiceTypeExchangeV1Identifier,
                fileServiceTypeExchangeV1Reader,
                fileServiceTypeExchangeV1Writer
            }
        }
    },

    {
        fileServiceTypeBlocks,
        EWM_BLOCK_VERSION_1,
        1,
        {
            {
                EWM_BLOCK_VERSION_1,
                fileServiceTypeBlockV1Identifier,
                fileServiceTypeBlockV1Reader,
                fileServiceTypeBlockV1Writer
            }
        }
    },

    {
        fileServiceTypeNodes,
        EWM_NODE_VERSION_1,
        1,
        {
            {
                EWM_NODE_VERSION_1,
                fileServiceTypeNodeV1Identifier,
                fileServiceTypeNodeV1Reader,
                fileServiceTypeNodeV1Writer
            }
        }
    },

    {
        fileServiceTypeTokens,
        EWM_TOKEN_VERSION_1,
        1,
        {
            {
                EWM_TOKEN_VERSION_1,
                fileServiceTypeTokenV1Identifier,
                fileServiceTypeTokenV1Reader,
                fileServiceTypeTokenV1Writer
            }
        }
    },

    {
        fileServiceTypeWallets,
        EWM_WALLET_VERSION_1,
        1,
        {
            {
                EWM_TOKEN_VERSION_1,
                fileServiceTypeWalletV1Identifier,
                fileServiceTypeWalletV1Reader,
                fileServiceTypeWalletV1Writer
            }
        }
    }
};

const char *ewmFileServiceTypeTransactions = fileServiceTypeTransactions;
const char *ewmFileServiceTypeLogs         = fileServiceTypeLogs;
const char *ewmFileServiceTypeExchanges    = fileServiceTypeExchanges;
const char *ewmFileServiceTypeBlocks       = fileServiceTypeBlocks;
const char *ewmFileServiceTypeNodes        = fileServiceTypeNodes;
const char *ewmFileServiceTypeTokens       = fileServiceTypeTokens;
const char *ewmFileServiceTypeWallets      = fileServiceTypeWallets;

size_t ewmFileServiceSpecificationsCount = (sizeof (fileServiceSpecifications) / sizeof (BRFileServiceTypeSpecification));
BRFileServiceTypeSpecification *ewmFileServiceSpecifications = fileServiceSpecifications;

