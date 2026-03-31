#include "UDPProtocol.hpp"
#include "Data.hpp"

#if defined(WIN32)
    #define NOMINMAX
    #include <winsock.h>
    using send_result_t = int;
#elif defined(__linux__) || defined(__APPLE__)
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket(x) close(x)
    using send_result_t = ssize_t;
#endif

#include <algorithm>

bool gelf::UDPProtocol::send(const struct sockaddr_in& pSockAddr, const ChunkedData& rData)
{
    if( rData.m_pData == nullptr || rData.m_uSize == 0 )
    {
        m_strError = "Invalid data: null or empty";
        return false;
    }

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);

    if(sock == INVALID_SOCKET)
    {
        m_strError = "Could not create socket";
        return false;
    }

    size_t uTotalBytesSent = 0;
    const uint8_t* pData = rData.m_pData;

    while(uTotalBytesSent < rData.m_uSize)
    {
        const size_t uBytesToSend = std::min(rData.m_uChunkSize, rData.m_uSize - uTotalBytesSent);
        const send_result_t iBytesSent = sendto(sock, reinterpret_cast<const char*>(pData), static_cast<int>(uBytesToSend), 0, reinterpret_cast<const struct sockaddr*>(&pSockAddr), static_cast<int>(sizeof(struct sockaddr_in)));

        if(iBytesSent == SOCKET_ERROR || static_cast<size_t>( iBytesSent ) != uBytesToSend)
        {
            m_strError = "Was not able to send data";
            closesocket(sock);
            return false;
        }

        uTotalBytesSent += static_cast<size_t>(iBytesSent);
        pData += iBytesSent;
    }

    closesocket( sock );
    return true;
}

const std::string gelf::UDPProtocol::getError() const
{
    return m_strError;
}
