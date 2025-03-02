#include <modbus/modbus.h>
#include <iostream>
#include <cstring>
#include <iomanip>

int main() {
    uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];

    // 创建 Modbus TCP 上下文
    modbus_t *ctx = modbus_new_tcp("127.0.0.1", 5020);
    if (ctx == nullptr) {
        std::cerr << "Unable to create the libmodbus context" << std::endl;
        return -1;
    }

    // 连接到服务端
    if (modbus_connect(ctx) == -1) {
        std::cerr << "Connection failed: " << modbus_strerror(errno) << std::endl;
        modbus_free(ctx);
        return -1;
    }

    std::cout << "Connected to server." << std::endl;

    uint8_t raw_req[] = {
        // MBAP 报文头
        0x00, 0x00,             // 事务标识符
        0x00, 0x00,             // 协议标识符
        0x00, 0x01,             // 数据总长度（PDU + CRC）
        0x01,                   // 单元标识符
        // PDU 报文体
        0x6E,                   // 功能码
        0x0A,                   // 命令码
        0x00,                   // 子命令码
        0x00, 0x01,             // 数据总长度（PDU + CRC）
        0x00, 0x00, 0x00, 0x00, // 数据总长度
        0x00, 0x00, 0x00, 0x00, // 数据偏移
        0x00,                   // 数据
        0x7B,                   // 校验和
    };

    // 发送自定义报文
    int rc = modbus_send_raw_request(ctx, raw_req,  22 * sizeof(uint8_t));
    if (rc == -1) {
        std::cerr << "Failed to send request: " << modbus_strerror(errno) << std::endl;
        modbus_close(ctx);
        modbus_free(ctx);
        return -1;
    }

    std::cout << "Request sent successfully." << std::endl;

    // 接收响应
    uint8_t rsp[MODBUS_TCP_MAX_ADU_LENGTH];
    rc = modbus_receive(ctx, rsp);
    if (rc > 0) {
        std::cout << "Received response: ";
        for (int i = 0; i < rc; ++i) {
            std::cout << "0x";
            std::cout << std::hex << std::uppercase << std::setfill('0') << std::setw(2) ;
            std::cout << static_cast<int>(rsp[i]) << " ";
        }
        std::cout << std::dec << std::endl;
    } else {
        std::cerr << "Failed to receive response: " << modbus_strerror(errno) << std::endl;
    }

    // 关闭连接并释放资源
    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}
