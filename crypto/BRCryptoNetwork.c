//
//  BRCryptoNetwork.c
//  BRCore
//
//  Created by Ed Gamble on 3/19/19.
//  Copyright © 2019 breadwallet. All rights reserved.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.

#include "BRCryptoNetwork.h"
#include "BRCryptoPrivate.h"

#include "bitcoin/BRChainParams.h"
#include "bcash/BRBCashParams.h"
#include "ethereum/BREthereum.h"

static void
cryptoNetworkRelease (BRCryptoNetwork network);

typedef struct {
    BRCryptoCurrency currency;
    BRCryptoUnit baseUnit;
    BRCryptoUnit defaultUnit;
    BRArrayOf(BRCryptoUnit) units;
} BRCryptoCurrencyAssociation;

#define CRYPTO_NETWORK_DEFAULT_CURRENCY_ASSOCIATIONS        (2)
#define CRYPTO_NETWORK_DEFAULT_NETWORKS                     (5)

struct BRCryptoNetworkRecord {
    char *uids;
    char *name;
    BRCryptoBlockChainHeight height;
    BRCryptoCurrency currency;
    BRArrayOf(BRCryptoCurrencyAssociation) associations;

    BRCryptoBlockChainType type;
    union {
        struct {
            uint8_t forkId;
            const BRChainParams *params;
        } btc;

        struct {
            uint32_t chainId;
            BREthereumNetwork net;
        } eth;

        struct {
        } gen;
    } u;
    BRCryptoRef ref;
};

IMPLEMENT_CRYPTO_GIVE_TAKE (BRCryptoNetwork, cryptoNetwork)

static BRCryptoNetwork
cryptoNetworkCreate (const char *uids,
                     const char *name) {
    BRCryptoNetwork network = malloc (sizeof (struct BRCryptoNetworkRecord));

    network->uids = strdup (uids);
    network->name = strdup (name);
    network->currency = NULL;
    network->height = 0;
    array_new (network->associations, CRYPTO_NETWORK_DEFAULT_CURRENCY_ASSOCIATIONS);
    network->ref = CRYPTO_REF_ASSIGN(cryptoNetworkRelease);

    return network;
}

private_extern BRCryptoNetwork
cryptoNetworkCreateAsBTC (const char *uids,
                          const char *name,
                          uint8_t forkId,
                          const BRChainParams *params) {
    BRCryptoNetwork network = cryptoNetworkCreate (uids, name);
    network->type = BLOCK_CHAIN_TYPE_BTC;
    network->u.btc.forkId = forkId;
    network->u.btc.params = params;

    return network;
}

private_extern BRCryptoNetwork
cryptoNetworkCreateAsETH (const char *uids,
                          const char *name,
                          uint32_t chainId,
                          BREthereumNetwork net) {
    BRCryptoNetwork network = cryptoNetworkCreate (uids, name);
    network->type = BLOCK_CHAIN_TYPE_ETH;
    network->u.eth.chainId = chainId;
    network->u.eth.net = net;

    return network;
}

private_extern BRCryptoNetwork
cryptoNetworkCreateAsGEN (const char *uids,
                          const char *name) {
    BRCryptoNetwork network = cryptoNetworkCreate (uids, name);
    network->type = BLOCK_CHAIN_TYPE_GEN;
    return network;
}

private_extern void
cryptoNetworkAnnounce (BRCryptoNetwork network) {
}

static void
cryptoNetworkRelease (BRCryptoNetwork network) {
    for (size_t index = 0; index < array_count (network->associations); index++) {
        BRCryptoCurrencyAssociation *association = &network->associations[index];
        cryptoCurrencyGive (association->currency);
        cryptoUnitGive (association->baseUnit);
        cryptoUnitGive (association->defaultUnit);
        cryptoUnitGiveAll (association->units);
    }

    // TBD
    switch (network->type){
        case BLOCK_CHAIN_TYPE_BTC:
            break;
        case BLOCK_CHAIN_TYPE_ETH:
            break;
        case BLOCK_CHAIN_TYPE_GEN:
            break;
    }

    free (network->name);
    cryptoCurrencyGive (network->currency);
    array_free (network->associations);
    free (network);
}

extern BRCryptoBlockChainType
cryptoNetworkGetType (BRCryptoNetwork network) {
    return network->type;
}

extern const char *
cryptoNetworkGetUids (BRCryptoNetwork network) {
    return network->uids;
}

extern const char *
cryptoNetworkGetName (BRCryptoNetwork network) {
    return network->name;
}

extern BRCryptoBoolean
cryptoNetworkIsMainnet (BRCryptoNetwork network) {
    switch (network->type) {
        case BLOCK_CHAIN_TYPE_BTC:
            return AS_CRYPTO_BOOLEAN (network->u.btc.params == BRMainNetParams ||
                                      network->u.btc.params == BRBCashParams);
        case BLOCK_CHAIN_TYPE_ETH:
            return AS_CRYPTO_BOOLEAN (network->u.eth.net == ethereumMainnet);
        case BLOCK_CHAIN_TYPE_GEN:
            return CRYPTO_TRUE;
    }
}

extern BRCryptoBlockChainHeight
cryptoNetworkGetHeight (BRCryptoNetwork network) {
    return network->height;
}

private_extern void
cryptoNetworkSetHeight (BRCryptoNetwork network,
                        BRCryptoBlockChainHeight height) {
    network->height = height;
}

extern BRCryptoCurrency
cryptoNetworkGetCurrency (BRCryptoNetwork network) {
    return network->currency;
}

private_extern void
cryptoNetworkSetCurrency (BRCryptoNetwork network,
                          BRCryptoCurrency currency) {
    network->currency = cryptoCurrencyTake(currency);
}

extern size_t
cryptoNetworkGetCurrencyCount (BRCryptoNetwork network) {
    return array_count (network->associations);
}

extern BRCryptoCurrency
cryptoNetworkGetCurrencyAt (BRCryptoNetwork network,
                            size_t index) {
    assert (index < array_count(network->associations));
    return network->associations[index].currency;
}

extern BRCryptoBoolean
cryptoNetworkHasCurrency (BRCryptoNetwork network,
                          BRCryptoCurrency currency) {
    for (size_t index = 0; index < array_count(network->associations); index++)
        if (CRYPTO_TRUE == cryptoCurrencyIsIdentical (network->associations[index].currency, currency))
            return CRYPTO_TRUE;
    return CRYPTO_FALSE;
}

extern BRCryptoCurrency
cryptoNetworkGetCurrencyForCode (BRCryptoNetwork network,
                                   const char *code) {
    for (size_t index = 0; index < array_count(network->associations); index++)
        if (0 == strcmp (code, cryptoCurrencyGetCode (network->associations[index].currency)))
            return network->associations[index].currency;
    return NULL;
}

static BRCryptoCurrencyAssociation *
cryptoNetworkLookupCurrency (BRCryptoNetwork network,
                             BRCryptoCurrency currency) {
    for (size_t index = 0; index < array_count(network->associations); index++)
        if (currency == network->associations[index].currency)
            return &network->associations[index];
    return NULL;
}

extern BRCryptoUnit
cryptoNetworkGetUnitAsBase (BRCryptoNetwork network,
                            BRCryptoCurrency currency) {
    BRCryptoCurrencyAssociation *association = cryptoNetworkLookupCurrency (network, currency);
    return NULL == association ? NULL : association->baseUnit;
}

extern BRCryptoUnit
cryptoNetworkGetUnitAsDefault (BRCryptoNetwork network,
                               BRCryptoCurrency currency) {
    BRCryptoCurrencyAssociation *association = cryptoNetworkLookupCurrency (network, currency);
    return NULL == association ? NULL : association->defaultUnit;
}

extern size_t
cryptoNetworkGetUnitCount (BRCryptoNetwork network,
                           BRCryptoCurrency currency) {
    BRCryptoCurrencyAssociation *association = cryptoNetworkLookupCurrency (network, currency);
    return ((NULL == association || NULL == association->units)
            ? 0
            : array_count(association->units));
}

extern BRCryptoUnit
cryptoNetworkGetUnitAt (BRCryptoNetwork network,
                        BRCryptoCurrency currency,
                        size_t index) {
    BRCryptoCurrencyAssociation *association = cryptoNetworkLookupCurrency (network, currency);
    return ((NULL == association || NULL == association->units || index >= array_count(association->units))
            ? 0
            : association->units[index]);
}

private_extern void
cryptoNetworkAddCurrency (BRCryptoNetwork network,
                          BRCryptoCurrency currency,
                          BRCryptoUnit baseUnit,
                          BRCryptoUnit defaultUnit) {
    BRCryptoCurrencyAssociation association = {
        cryptoCurrencyTake (currency),
        cryptoUnitTake (baseUnit),
        cryptoUnitTake (defaultUnit),
        NULL
    };
    array_new (association.units, 2);

    array_add (network->associations, association);
}

private_extern void
cryptoNetworkAddCurrencyUnit (BRCryptoNetwork network,
                              BRCryptoCurrency currency,
                              BRCryptoUnit unit) {
    BRCryptoCurrencyAssociation *association = cryptoNetworkLookupCurrency (network, currency);
    if (NULL != association)
        array_add (association->units, cryptoUnitTake (unit));
}

private_extern BREthereumNetwork
cryptoNetworkAsETH (BRCryptoNetwork network) {
    assert (BLOCK_CHAIN_TYPE_ETH == network->type);
    return network->u.eth.net;
}

private_extern const BRChainParams *
cryptoNetworkAsBTC (BRCryptoNetwork network) {
    assert (BLOCK_CHAIN_TYPE_BTC == network->type);
    return network->u.btc.params;
}

private_extern BRCryptoBlockChainType
cryptoNetworkGetBlockChainType (BRCryptoNetwork network) {
    return network->type;
}
