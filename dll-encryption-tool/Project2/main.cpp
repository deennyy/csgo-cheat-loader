#include "../../cryptopp860/modes.h"
#include "../../cryptopp860/aes.h"
#include "../../cryptopp860/hex.h"
#include "../../cryptopp860/files.h"

// https://cryptopp.com/wiki/Advanced_Encryption_Standard
int main() {
    using namespace CryptoPP;

    std::string path;

    std::cin >> path;

    std::ifstream in{ path, std::ios::binary };
    std::ofstream out{ "cheat.dll", std::ios::binary };

    try
    {
        CBC_Mode< AES >::Encryption e;
        e.SetKeyWithIV((byte*)"REDACTED", 16, (byte*)"REDACTED");
        FileSource f{ in, true, new CryptoPP::StreamTransformationFilter{e, new HexEncoder (new CryptoPP::FileSink{out})}};
    }
    catch (const Exception& e)
    {
        std::cerr << e.what() << std::endl;
        exit(1);
    }
}