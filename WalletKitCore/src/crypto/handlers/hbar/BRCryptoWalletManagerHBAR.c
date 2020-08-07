//
//  BRCryptoWalletManagerHBAR.c
//  Core
//
//  Created by Ehsan Rezaie on 2020-05-19.
//  Copyright © 2019 Breadwallet AG. All rights reserved.
//
//  See the LICENSE file at the project root for license information.
//  See the CONTRIBUTORS file at the project root for a list of contributors.
//
#include "BRCryptoHBAR.h"

#include "crypto/BRCryptoAccountP.h"
#include "crypto/BRCryptoNetworkP.h"
#include "crypto/BRCryptoKeyP.h"
#include "crypto/BRCryptoClientP.h"
#include "crypto/BRCryptoWalletP.h"
#include "crypto/BRCryptoAmountP.h"
#include "crypto/BRCryptoWalletManagerP.h"
#include "crypto/BRCryptoFileService.h"

#include "hedera/BRHederaWallet.h"
#include "hedera/BRHederaAccount.h"


// MARK: - Events

//TODO:HBAR make common
const BREventType *hbarEventTypes[] = {
    // ...
};

const unsigned int
hbarEventTypesCount = (sizeof (hbarEventTypes) / sizeof (BREventType*));


static BRCryptoWalletManagerHBAR
cryptoWalletManagerCoerce (BRCryptoWalletManager wm) {
    assert (CRYPTO_NETWORK_TYPE_HBAR == wm->type);
    return (BRCryptoWalletManagerHBAR) wm;
}

// MARK: - Handlers

static BRCryptoWalletManager
cryptoWalletManagerCreateHandlerHBAR (BRCryptoListener listener,
                                      BRCryptoClient client,
                                      BRCryptoAccount account,
                                      BRCryptoNetwork network,
                                      BRCryptoSyncMode mode,
                                      BRCryptoAddressScheme scheme,
                                      const char *path) {
    BRCryptoWalletManager manager = cryptoWalletManagerAllocAndInit (sizeof (struct BRCryptoWalletManagerHBARRecord),
                                                                     cryptoNetworkGetType(network),
                                                                     listener,
                                                                     client,
                                                                     account,
                                                                     network,
                                                                     scheme,
                                                                     path,
                                                                     CRYPTO_CLIENT_REQUEST_USE_TRANSFERS,
                                                                     NULL,
                                                                     NULL);

    // Create the primary wallet
    manager->wallet = cryptoWalletManagerCreateWallet (manager, network->currency);

    pthread_mutex_unlock (&manager->lock);
    return manager;
}

static void
cryptoWalletManagerReleaseHandlerHBAR (BRCryptoWalletManager manager) {
    
}

static BRFileService
crytpWalletManagerCreateFileServiceHBAR (BRCryptoWalletManager manager,
                                         const char *basePath,
                                         const char *currency,
                                         const char *network,
                                         BRFileServiceContext context,
                                         BRFileServiceErrorHandler handler) {
    return fileServiceCreateFromTypeSpecfications (basePath, currency, network,
                                                   context, handler,
                                                   fileServiceSpecificationsCount,
                                                   fileServiceSpecifications);
}

static const BREventType **
cryptoWalletManagerGetEventTypesHBAR (BRCryptoWalletManager manager,
                                      size_t *eventTypesCount) {
    assert (NULL != eventTypesCount);
    *eventTypesCount = hbarEventTypesCount;
    return hbarEventTypes;
}

static BRCryptoBoolean
cryptoWalletManagerSignTransactionWithSeedHandlerHBAR (BRCryptoWalletManager manager,
                                                       BRCryptoWallet wallet,
                                                       BRCryptoTransfer transfer,
                                                       UInt512 seed) {
    BRHederaAccount account = cryptoAccountAsHBAR (manager->account);
    BRKey publicKey = hederaAccountGetPublicKey (account);
    BRHederaTransaction transaction = cryptoTransferCoerceHBAR(transfer)->hbarTransaction;
    size_t tx_size = hederaTransactionSignTransaction (transaction, publicKey, seed);
    return AS_CRYPTO_BOOLEAN(tx_size > 0);
}

static BRCryptoBoolean
cryptoWalletManagerSignTransactionWithKeyHandlerHBAR (BRCryptoWalletManager manager,
                                                      BRCryptoWallet wallet,
                                                      BRCryptoTransfer transfer,
                                                      BRCryptoKey key) {
    assert(0);
    return CRYPTO_FALSE;
}

//TODO:HBAR make common?
static BRCryptoAmount
cryptoWalletManagerEstimateLimitHandlerHBAR (BRCryptoWalletManager manager,
                                             BRCryptoWallet  wallet,
                                             BRCryptoBoolean asMaximum,
                                             BRCryptoAddress target,
                                             BRCryptoNetworkFee networkFee,
                                             BRCryptoBoolean *needEstimate,
                                             BRCryptoBoolean *isZeroIfInsuffientFunds,
                                             BRCryptoUnit unit) {
    UInt256 amount = UINT256_ZERO;
    
    *needEstimate = CRYPTO_FALSE;
    *isZeroIfInsuffientFunds = CRYPTO_FALSE;
    
    if (CRYPTO_TRUE == asMaximum) {
        BRCryptoAmount minBalance = wallet->balanceMinimum;
        assert(minBalance);
        
        // Available balance based on minimum wallet balance
        BRCryptoAmount balance = cryptoAmountSub(wallet->balance, minBalance);
        
        // Hedera has fixed network fee (costFactor = 1.0)
        BRCryptoAmount fee = cryptoNetworkFeeGetPricePerCostFactor (networkFee);
        BRCryptoAmount newBalance = cryptoAmountSub(balance, fee);
        
        if (CRYPTO_TRUE == cryptoAmountIsNegative(newBalance)) {
            amount = UINT256_ZERO;
        } else {
            amount = cryptoAmountGetValue(newBalance);
        }
        
        cryptoAmountGive (balance);
        cryptoAmountGive (fee);
        cryptoAmountGive (newBalance);
    }
    
    return cryptoAmountCreateInternal (unit,
                                       CRYPTO_FALSE,
                                       amount,
                                       0);
}

static BRCryptoFeeBasis
cryptoWalletManagerEstimateFeeBasisHandlerHBAR (BRCryptoWalletManager manager,
                                                BRCryptoWallet  wallet,
                                                BRCryptoCookie cookie,
                                                BRCryptoAddress target,
                                                BRCryptoAmount amount,
                                                BRCryptoNetworkFee networkFee) {
    BRCryptoAmount pricePerCostFactor = cryptoNetworkFeeGetPricePerCostFactor (networkFee);
    double costFactor = 1.0;  // 'cost factor' is 'transaction'

    return cryptoFeeBasisCreate (pricePerCostFactor, costFactor);
}

static void
cryptoWalletManagerRecoverTransfersFromTransactionBundleHandlerHBAR (BRCryptoWalletManager manager,
                                                                     OwnershipKept BRCryptoClientTransactionBundle bundle) {
    // Not Hedera functionality
    assert (0);
}

static void
cryptoWalletManagerRecoverTransferFromTransferBundleHandlerHBAR (BRCryptoWalletManager manager,
                                                                 OwnershipKept BRCryptoClientTransferBundle bundle) {
    // create BRHederaTransaction
    
    BRHederaWallet hbarWallet = cryptoWalletAsHBAR (manager->wallet);
    
    BRHederaUnitTinyBar amountHbar, feeHbar = 0;
    sscanf(bundle->amount, "%" PRIi64, &amountHbar);
    if (NULL != bundle->fee) sscanf(bundle->fee, "%" PRIi64, &feeHbar);
    BRHederaAddress toAddress   = hederaAddressCreateFromString(bundle->to,   false);
    BRHederaAddress fromAddress = hederaAddressCreateFromString(bundle->from, false);
    // Convert the hash string to bytes
    BRHederaTransactionHash txId;
    hexDecode(txId.bytes, sizeof(txId.bytes), bundle->hash, strlen(bundle->hash));
    
    BRHederaTransactionHash txHash;
    memset(txHash.bytes, 0x00, sizeof(txHash.bytes));
    if (bundle->hash != NULL) {
        assert(96 == strlen(bundle->hash));
        hexDecode(txHash.bytes, sizeof(txHash.bytes), bundle->hash, strlen(bundle->hash));
    }
    
    int error = (CRYPTO_TRANSFER_STATE_ERRORED == bundle->status);
    
    BRHederaTransaction hbarTransaction = hederaTransactionCreate(fromAddress,
                                                                  toAddress,
                                                                  amountHbar,
                                                                  feeHbar,
                                                                  NULL,
                                                                  txHash,
                                                                  bundle->blockTimestamp,
                                                                  bundle->blockNumber,
                                                                  error);
    
    hederaAddressFree (toAddress);
    hederaAddressFree (fromAddress);
    
    hederaWalletAddTransfer (hbarWallet, hbarTransaction); //TODO:HBAR needed?
    
    // create BRCryptoTransfer
    
    BRCryptoWallet wallet = cryptoWalletManagerGetWallet (manager);

    BRCryptoTransfer baseTransfer = cryptoTransferCreateAsHBAR (wallet->listenerTransfer,
                                                                wallet->unit,
                                                                wallet->unitForFee,
                                                                hbarWallet,
                                                                hbarTransaction);
    cryptoWalletAddTransfer (wallet, baseTransfer);
    
    //TODO:HBAR attributes
    //TODO:HBAR save to fileService
    //TODO:HBAR announce
    
    hederaTransactionFree (hbarTransaction);
}

extern BRCryptoWalletSweeperStatus
cryptoWalletManagerWalletSweeperValidateSupportedHBAR (BRCryptoWalletManager manager,
                                                       BRCryptoWallet wallet,
                                                       BRCryptoKey key) {
    return CRYPTO_WALLET_SWEEPER_UNSUPPORTED_CURRENCY;
}

extern BRCryptoWalletSweeper
cryptoWalletManagerCreateWalletSweeperHBAR (BRCryptoWalletManager manager,
                                            BRCryptoWallet wallet,
                                            BRCryptoKey key) {
    // not supported
    return NULL;
}

static BRCryptoWallet
cryptoWalletManagerCreateWalletHBAR (BRCryptoWalletManager manager,
                                       BRCryptoCurrency currency) {
    BRHederaAccount hbarAccount = cryptoAccountAsHBAR(manager->account);
    BRHederaWallet hbarWallet = hederaWalletCreate(hbarAccount);

    // Create the primary BRCryptoWallet
    BRCryptoNetwork  network       = manager->network;
    BRCryptoUnit     unitAsBase    = cryptoNetworkGetUnitAsBase    (network, currency);
    BRCryptoUnit     unitAsDefault = cryptoNetworkGetUnitAsDefault (network, currency);

    BRCryptoWallet wallet = cryptoWalletCreateAsHBAR (manager->listenerWallet,
                                                unitAsDefault,
                                                unitAsDefault,
                                                hbarWallet);
    cryptoWalletManagerAddWallet (manager, wallet);

    //TODO:HBAR load transfers from fileService

    cryptoUnitGive (unitAsDefault);
    cryptoUnitGive (unitAsBase);

    return wallet;
}

BRCryptoWalletManagerHandlers cryptoWalletManagerHandlersHBAR = {
    cryptoWalletManagerCreateHandlerHBAR,
    cryptoWalletManagerReleaseHandlerHBAR,
    crytpWalletManagerCreateFileServiceHBAR,
    cryptoWalletManagerGetEventTypesHBAR,
    cryptoWalletManagerCreateWalletHBAR,
    cryptoWalletManagerSignTransactionWithSeedHandlerHBAR,
    cryptoWalletManagerSignTransactionWithKeyHandlerHBAR,
    cryptoWalletManagerEstimateLimitHandlerHBAR,
    cryptoWalletManagerEstimateFeeBasisHandlerHBAR,
    cryptoWalletManagerRecoverTransfersFromTransactionBundleHandlerHBAR,
    cryptoWalletManagerRecoverTransferFromTransferBundleHandlerHBAR,
    cryptoWalletManagerWalletSweeperValidateSupportedHBAR,
    cryptoWalletManagerCreateWalletSweeperHBAR
};