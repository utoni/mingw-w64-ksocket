#include <aes/aes256.hpp>

extern "C" {
#include <ksocket/berkeley.h>
#include <ksocket/ksocket.h>
#include <ksocket/wsk.h>

DRIVER_INITIALIZE DriverEntry;
DRIVER_UNLOAD DriverUnload;

#define DebuggerPrint(...)                                                     \
  DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, __VA_ARGS__);

NTSTATUS
NTAPI
DriverEntry(_In_ PDRIVER_OBJECT DriverObject,
            _In_ PUNICODE_STRING RegistryPath) {
  UNREFERENCED_PARAMETER(DriverObject);
  UNREFERENCED_PARAMETER(RegistryPath);

  {
    ByteArray key = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                     0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                     0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};
    ByteArray enc, dec;
    const char secret_message[] = "This is a top secret message.";
    const size_t secret_message_len = strlen(secret_message);

    enc.clear();
    dec.clear();

    {
      Aes256 aes(key);
      aes.encrypt_start(secret_message_len, enc);
      aes.encrypt_continue((unsigned char *)secret_message, secret_message_len,
                           enc);
      aes.encrypt_end(enc);
    }

    {
      Aes256 aes(key);
      aes.decrypt_start(enc.size());
      aes.decrypt_continue(enc.data(), enc.size(), dec);
      aes.decrypt_end(dec);
    }

    if (memcmp(secret_message, dec.data(), secret_message_len) != 0) {
      DebuggerPrint("%s\n", "AES secret message differs!");
      DebuggerPrint("Original.: %s\n", secret_message);
    }
    DebuggerPrint("Decrypted: %s\n", dec.data());
  }

  return STATUS_SUCCESS;
}

VOID DriverUnload(_In_ struct _DRIVER_OBJECT *DriverObject) {
  UNREFERENCED_PARAMETER(DriverObject);

  DebuggerPrint("Bye.");
}
}
