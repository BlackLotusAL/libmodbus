#include <iomanip>
#include <modbus/modbus.h>
#include <iostream>

int main() {
    int server_socket = -1;

    // 创建 Modbus TCP 上下文
    modbus_t *ctx = modbus_new_tcp("127.0.0.1", 5020);
    if (ctx == nullptr) {
        std::cerr << "Unable to create the libmodbus context" << std::endl;
        return -1;
    }

    // 创建数据映射
    modbus_mapping_t *mapping = modbus_mapping_new(100, 100, 100, 100);
    if (mapping == nullptr) {
        std::cerr << "Failed to allocate the mapping: " << modbus_strerror(errno) << std::endl;
        modbus_free(ctx);
        return -1;
    }

    // 初始化部分寄存器数据
    for (int i = 0; i < 10; ++i) {
        mapping->tab_registers[i] = i * 10;
    }

    // 开始监听
    server_socket = modbus_tcp_listen(ctx, 5);
    if (server_socket == -1) {
        std::cerr << "Failed to listen on port: " << modbus_strerror(errno) << std::endl;
        modbus_mapping_free(mapping);
        modbus_free(ctx);
        return -1;
    }

    std::cout << "Server is listening on port 5020..." << std::endl;

    while (true) {
        int client_socket = -1;

        // 接受客户端连接
        client_socket = modbus_tcp_accept(ctx, &server_socket);
        if (client_socket == -1) {
            std::cerr << "Failed to accept connection: " << modbus_strerror(errno) << std::endl;
            continue;
        }

        std::cout << "Client connected." << std::endl;

        uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];

        // 接收客户端请求
        const int rc = modbus_receive(ctx, query);
        if (rc > 0) {
            std::cout << "Received request: ";
            for (int i = 0; i < rc; ++i) {
                std::cout << "0x";
                std::cout << std::hex << std::uppercase << std::setfill('0') << std::setw(2) ;
                std::cout << static_cast<int>(query[i]) << " ";
            }
            std::cout << std::dec << std::endl;

            // 处理请求并返回响应
            modbus_reply(ctx, query, rc, mapping);
        } else {
            std::cerr << "Failed to receive request: " << modbus_strerror(errno) << std::endl;
        }

        // 关闭客户端连接
        close(client_socket);
        std::cout << "Client disconnected." << std::endl;
    }

    // 清理资源
    modbus_mapping_free(mapping);
    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}