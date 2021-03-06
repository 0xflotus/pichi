#ifndef PICHI_CRYPTO_AEAD_HPP
#define PICHI_CRYPTO_AEAD_HPP

#include <array>
#include <mbedtls/gcm.h>
#include <pichi/buffer.hpp>
#include <pichi/crypto/method.hpp>

namespace pichi::crypto {

/*
 * For AES-GCMs: context means mbedtls context
 * For CHACHA20 and XCHACHA20: context means subkey generated by HKDF_SHA1
 */
template <CryptoMethod method>
using AeadContext =
    std::conditional_t<helpers::isGcm<method>(), mbedtls_gcm_context,
                       std::conditional_t<helpers::isSodiumAead<method>(),
                                          std::array<uint8_t, KEY_SIZE<method>>, void>>;

template <CryptoMethod method> class AeadEncryptor {
public:
  static_assert(helpers::isAead<method>(), "Not an AEAD crypto method");

  AeadEncryptor(AeadEncryptor const&) = delete;
  AeadEncryptor(AeadEncryptor&&) = delete;
  AeadEncryptor& operator=(AeadEncryptor const&) = delete;
  AeadEncryptor& operator=(AeadEncryptor&&) = delete;

public:
  AeadEncryptor(ConstBuffer<uint8_t> key, ConstBuffer<uint8_t> iv = {});
  ~AeadEncryptor();

  ConstBuffer<uint8_t> getIv() const;
  size_t encrypt(ConstBuffer<uint8_t> plain, MutableBuffer<uint8_t> cipher);

private:
  std::array<uint8_t, NONCE_SIZE<method>> nonce_;
  std::array<uint8_t, IV_SIZE<method>> salt_;
  AeadContext<method> ctx_;
};

template <CryptoMethod method> class AeadDecryptor {
public:
  static_assert(helpers::isAead<method>(), "Not an AEAD crypto method");

  AeadDecryptor(AeadDecryptor const&) = delete;
  AeadDecryptor(AeadDecryptor&&) = delete;
  AeadDecryptor& operator=(AeadDecryptor const&) = delete;
  AeadDecryptor& operator=(AeadDecryptor&&) = delete;

public:
  AeadDecryptor(ConstBuffer<uint8_t> key);
  ~AeadDecryptor();

  size_t getIvSize() const;
  void setIv(ConstBuffer<uint8_t> iv);
  size_t decrypt(ConstBuffer<uint8_t> cipher, MutableBuffer<uint8_t> plain);

private:
  std::array<uint8_t, KEY_SIZE<method>> ikm_;
  std::array<uint8_t, NONCE_SIZE<method>> nonce_;
  AeadContext<method> ctx_;
  bool initialized_ = false;
};

} // namespace pichi::crypto

#endif
