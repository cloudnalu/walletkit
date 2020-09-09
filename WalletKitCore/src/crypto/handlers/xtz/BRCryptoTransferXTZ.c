//
//  BRCryptoTransferXTZ.c
//  Core
//
//  Created by Ehsan Rezaie on 2020-08-27.
//  Copyright © 2019 Breadwallet AG. All rights reserved.
//
//  See the LICENSE file at the project root for license information.
//  See the CONTRIBUTORS file at the project root for a list of contributors.
//
#include "BRCryptoXTZ.h"
#include "crypto/BRCryptoAmountP.h"
#include "crypto/BRCryptoHashP.h"
#include "tezos/BRTezosTransfer.h"
#include "ethereum/util/BRUtilMath.h"

static BRCryptoTransferDirection
transferGetDirectionFromXTZ (BRTezosTransfer transfer,
                             BRTezosAccount account);

extern BRCryptoTransferXTZ
cryptoTransferCoerceXTZ (BRCryptoTransfer transfer) {
    assert (CRYPTO_NETWORK_TYPE_XTZ == transfer->type);
    return (BRCryptoTransferXTZ) transfer;
}

typedef struct {
    BRTezosTransfer xtzTransfer;
} BRCryptoTransferCreateContextXTZ;

static void
cryptoTransferCreateCallbackXTZ (BRCryptoTransferCreateContext context,
                                    BRCryptoTransfer transfer) {
    BRCryptoTransferCreateContextXTZ *contextXTZ = (BRCryptoTransferCreateContextXTZ*) context;
    BRCryptoTransferXTZ transferXTZ = cryptoTransferCoerceXTZ (transfer);

    transferXTZ->xtzTransfer = contextXTZ->xtzTransfer;
}

extern BRCryptoTransfer
cryptoTransferCreateAsXTZ (BRCryptoTransferListener listener,
                           BRCryptoUnit unit,
                           BRCryptoUnit unitForFee,
                           OwnershipKept BRTezosAccount xtzAccount,
                           OwnershipGiven BRTezosTransfer xtzTransfer) {
    
    BRCryptoTransferDirection direction = transferGetDirectionFromXTZ (xtzTransfer, xtzAccount);
    
    BRCryptoAmount amount = cryptoAmountCreateAsXTZ (unit,
                                                     CRYPTO_FALSE,
                                                     tezosTransferGetAmount (xtzTransfer));
    
    BRCryptoFeeBasis feeBasisEstimated = cryptoFeeBasisCreateAsXTZ (unitForFee,
                                                                    tezosTransferGetFeeBasis (xtzTransfer));
    
    BRCryptoAddress sourceAddress = cryptoAddressCreateAsXTZ (tezosTransferGetSource (xtzTransfer));
    BRCryptoAddress targetAddress = cryptoAddressCreateAsXTZ (tezosTransferGetTarget (xtzTransfer));

    BRCryptoTransferCreateContextXTZ contextXTZ = {
        xtzTransfer
    };

    BRCryptoTransfer transfer = cryptoTransferAllocAndInit (sizeof (struct BRCryptoTransferXTZRecord),
                                                            CRYPTO_NETWORK_TYPE_XTZ,
                                                            listener,
                                                            unit,
                                                            unitForFee,
                                                            feeBasisEstimated,
                                                            amount,
                                                            direction,
                                                            sourceAddress,
                                                            targetAddress,
                                                            &contextXTZ,
                                                            cryptoTransferCreateCallbackXTZ);
    
    cryptoFeeBasisGive (feeBasisEstimated);
    cryptoAddressGive (sourceAddress);
    cryptoAddressGive (targetAddress);

    return transfer;
}

static void
cryptoTransferReleaseXTZ (BRCryptoTransfer transfer) {
    BRCryptoTransferXTZ transferXTZ = cryptoTransferCoerceXTZ(transfer);
    tezosTransferFree (transferXTZ->xtzTransfer);
}

static BRCryptoHash
cryptoTransferGetHashXTZ (BRCryptoTransfer transfer) {
    BRCryptoTransferXTZ transferXTZ = cryptoTransferCoerceXTZ(transfer);
    BRTezosHash hash = tezosTransferGetTransactionId (transferXTZ->xtzTransfer);
    return cryptoHashCreateInternal (CRYPTO_NETWORK_TYPE_XTZ, sizeof (hash.bytes), hash.bytes);
}

static uint8_t *
cryptoTransferSerializeXTZ (BRCryptoTransfer transfer,
                            BRCryptoNetwork network,
                            BRCryptoBoolean  requireSignature,
                            size_t *serializationCount) {
    BRCryptoTransferXTZ transferXTZ = cryptoTransferCoerceXTZ (transfer);

    uint8_t *serialization = NULL;
    *serializationCount = 0;
    BRTezosTransaction transaction = tezosTransferGetTransaction (transferXTZ->xtzTransfer);
    if (transaction) {
        serialization = tezosTransactionGetSignedBytes (transaction, serializationCount);
    }
    
    return serialization;
}

static int
cryptoTransferIsEqualXTZ (BRCryptoTransfer tb1, BRCryptoTransfer tb2) {
    return (tb1 == tb2 ||
            cryptoHashEqual (cryptoTransferGetHashXTZ(tb1),
                             cryptoTransferGetHashXTZ(tb2)));
}

static BRCryptoTransferDirection
transferGetDirectionFromXTZ (BRTezosTransfer transfer,
                             BRTezosAccount account) {
    BRTezosAddress source = tezosTransferGetSource (transfer);
    BRTezosAddress target = tezosTransferGetTarget (transfer);
    
    int isSource = tezosAccountHasAddress (account, source);
    int isTarget = tezosAccountHasAddress (account, target);
    
    return (isSource && isTarget
            ? CRYPTO_TRANSFER_RECOVERED
            : (isSource
               ? CRYPTO_TRANSFER_SENT
               : CRYPTO_TRANSFER_RECEIVED));
}

BRCryptoTransferHandlers cryptoTransferHandlersXTZ = {
    cryptoTransferReleaseXTZ,
    cryptoTransferGetHashXTZ,
    cryptoTransferSerializeXTZ,
    cryptoTransferIsEqualXTZ
};
