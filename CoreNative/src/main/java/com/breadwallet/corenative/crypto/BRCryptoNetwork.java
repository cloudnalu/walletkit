/*
 * Created by Michael Carrara <michael.carrara@breadwallet.com> on 7/1/19.
 * Copyright (c) 2019 Breadwinner AG.  All right reserved.
*
 * See the LICENSE file at the project root for license information.
 * See the CONTRIBUTORS file at the project root for a list of contributors.
 */
package com.breadwallet.corenative.crypto;

import com.breadwallet.corenative.CryptoLibrary;
import com.breadwallet.corenative.utility.SizeT;
import com.breadwallet.corenative.utility.SizeTByReference;
import com.google.common.base.Optional;
import com.google.common.primitives.UnsignedBytes;
import com.google.common.primitives.UnsignedInteger;
import com.google.common.primitives.UnsignedInts;
import com.google.common.primitives.UnsignedLong;
import com.sun.jna.Native;
import com.sun.jna.Pointer;
import com.sun.jna.PointerType;
import com.sun.jna.ptr.PointerByReference;

import java.util.ArrayList;
import java.util.List;

public class BRCryptoNetwork extends PointerType implements CoreBRCryptoNetwork {

    public BRCryptoNetwork(Pointer address) {
        super(address);
    }

    public BRCryptoNetwork() {
        super();
    }

    @Override
    public BRCryptoCurrency getCurrency() {
        return CryptoLibrary.INSTANCE.cryptoNetworkGetCurrency(this);
    }

    @Override
    public void setCurrency(BRCryptoCurrency currency) {
        CryptoLibrary.INSTANCE.cryptoNetworkSetCurrency(this, currency);
    }

    @Override
    public boolean hasCurrency(BRCryptoCurrency currency) {
        return BRCryptoBoolean.CRYPTO_TRUE == CryptoLibrary.INSTANCE.cryptoNetworkHasCurrency(this, currency);
    }

    @Override
    public UnsignedLong getCurrencyCount() {
        return UnsignedLong.fromLongBits(CryptoLibrary.INSTANCE.cryptoNetworkGetCurrencyCount(this).longValue());
    }

    @Override
    public BRCryptoCurrency getCurrency(UnsignedLong index) {
        return CryptoLibrary.INSTANCE.cryptoNetworkGetCurrencyAt(this,
                new SizeT(index.longValue()));
    }

    @Override
    public List<BRCryptoNetworkFee> getFees() {
        List<BRCryptoNetworkFee> fees = new ArrayList<>();
        SizeTByReference count = new SizeTByReference();
        Pointer feesPtr = CryptoLibrary.INSTANCE.cryptoNetworkGetNetworkFees(this, count);
        if (null != feesPtr) {
            try {
                int feesSize = UnsignedInts.checkedCast(count.getValue().longValue());
                for (Pointer feePtr: feesPtr.getPointerArray(0, feesSize)) {
                    fees.add(new BRCryptoNetworkFee.OwnedBRCryptoNetworkFee(feePtr));
                }

            } finally {
                Native.free(Pointer.nativeValue(feesPtr));
            }
        }
        return fees;
    }

    @Override
    public void setFees(List<BRCryptoNetworkFee> fees) {
        BRCryptoNetworkFee[] cryptoFees = new BRCryptoNetworkFee[fees.size()];
        for (int i = 0; i < fees.size(); i++) cryptoFees[i] = fees.get(i);

        CryptoLibrary.INSTANCE.cryptoNetworkSetNetworkFees(this, cryptoFees, new SizeT(cryptoFees.length));
    }

    @Override
    public String getUids() {
        return CryptoLibrary.INSTANCE.cryptoNetworkGetUids(this).getString(0, "UTF-8");
    }

    @Override
    public boolean isMainnet() {
        return BRCryptoBoolean.CRYPTO_TRUE == CryptoLibrary.INSTANCE.cryptoNetworkIsMainnet(this);
    }

    @Override
    public UnsignedLong getHeight() {
        return UnsignedLong.fromLongBits(CryptoLibrary.INSTANCE.cryptoNetworkGetHeight(this));
    }

    @Override
    public void setHeight(UnsignedLong height) {
        CryptoLibrary.INSTANCE.cryptoNetworkSetHeight(this, height.longValue());
    }

    @Override
    public UnsignedInteger getConfirmationsUntilFinal() {
        return UnsignedInteger.fromIntBits(CryptoLibrary.INSTANCE.cryptoNetworkGetConfirmationsUntilFinal(this));
    }

    @Override
    public void setConfirmationsUntilFinal(UnsignedInteger confirmationsUntilFinal) {
        CryptoLibrary.INSTANCE.cryptoNetworkSetConfirmationsUntilFinal(this, confirmationsUntilFinal.intValue());
    }

    @Override
    public String getName() {
        return CryptoLibrary.INSTANCE.cryptoNetworkGetName(this).getString(0, "UTF-8");
    }

    @Override
    public void addFee(BRCryptoNetworkFee fee) {
        CryptoLibrary.INSTANCE.cryptoNetworkAddNetworkFee(this, fee);
    }

    @Override
    public void addCurrency(BRCryptoCurrency currency, BRCryptoUnit baseUnit, BRCryptoUnit defaultUnit) {
        CryptoLibrary.INSTANCE.cryptoNetworkAddCurrency(this, currency, baseUnit, defaultUnit);
    }

    @Override
    public void addCurrencyUnit(BRCryptoCurrency currency, BRCryptoUnit unit) {
        CryptoLibrary.INSTANCE.cryptoNetworkAddCurrencyUnit(this, currency, unit);
    }

    @Override
    public Optional<BRCryptoUnit> getUnitAsBase(BRCryptoCurrency currency) {
        return Optional.fromNullable(CryptoLibrary.INSTANCE.cryptoNetworkGetUnitAsBase(this, currency));
    }

    @Override
    public Optional<BRCryptoUnit> getUnitAsDefault(BRCryptoCurrency currency) {
        return Optional.fromNullable(CryptoLibrary.INSTANCE.cryptoNetworkGetUnitAsDefault(this, currency));
    }

    @Override
    public UnsignedLong getUnitCount(BRCryptoCurrency currency) {
        return UnsignedLong.fromLongBits(CryptoLibrary.INSTANCE.cryptoNetworkGetUnitCount(this, currency).longValue());
    }

    @Override
    public Optional<BRCryptoUnit> getUnitAt(BRCryptoCurrency currency, UnsignedLong index) {
        return Optional.fromNullable(CryptoLibrary.INSTANCE.cryptoNetworkGetUnitAt(this, currency, new SizeT(index.longValue())));
    }

    @Override
    public Optional<BRCryptoAddress> addressFor(String address) {
        return Optional.fromNullable(CryptoLibrary.INSTANCE.cryptoNetworkCreateAddressFromString(this, address));
    }

    @Override
    public BRCryptoNetwork asBRCryptoNetwork() {
        return this;
    }
}
