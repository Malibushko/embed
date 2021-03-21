#pragma once
#include <etl/circular_buffer.h>

class Uart final {
    using handle_t = __UART_HandleTypeDef;
    using return_t = void;

    etl::circular_buffer<unsigned char,64> m_RxBuffer;
    etl::circular_buffer<unsigned char,64> m_TxBuffer;
    
    handle_t*                      m_UartHandle;
    
    bool m_IsReady { false };
    bool m_HasData { false };
public:
    Uart() = default;
    Uart(handle_t* _Handle) : m_UartHandle{_Handle} 
    {
        // Empty
    }
    
    void Init() {
        __HAL_UART_ENABLE_IT(m_UartHandle, UART_IT_ERR);
        __HAL_UART_ENABLE_IT(m_UartHandle, UART_IT_RXNE);
    }

    void Clear() {
        m_RxBuffer.clear();
        m_TxBuffer.clear();
    }
    
    bool WaitForData(const char * _Data, size_t _TimeoutMs) 
    {
        return false;
    }

    void HandleIRQ() {
        uint32_t SR_Flags  = READ_REG(m_UartHandle->Instance->SR);
        uint32_t CR1_Flags = READ_REG(m_UartHandle->Instance->CR1);

        if (((SR_Flags  & USART_SR_RXNE) != RESET) && 
            ((CR1_Flags & USART_CR1_RXNEIE) != RESET)) 
             {
                 m_UartHandle->Instance->SR;
                 unsigned char Data = m_UartHandle->Instance->DR;
                
                 m_RxBuffer.push(Data);
                 return;
             }
        

        if (((SR_Flags & USART_SR_TXE) != RESET) && 
            ((CR1_Flags & USART_CR1_TXEIE) != RESET))
            {
                if (!m_TxBuffer.empty())
                {
                    unsigned char Data = m_TxBuffer.back();
                    m_TxBuffer.pop();

                    m_UartHandle->Instance->SR;
                    m_UartHandle->Instance->DR = Data;
                }
                else
                    __HAL_UART_DISABLE_IT(m_UartHandle, UART_IT_TXE);

                return;
            }
    }

    return_t Write(const char * _Data)
    {
        while(*_Data) WriteChar(*_Data++);
    }

    return_t WriteChar(char _Data) {
        m_TxBuffer.push(_Data);
        __HAL_UART_ENABLE_IT(m_UartHandle, UART_IT_TXE);
        // HAL_Delay(1); -- Это фиксит проблему
    }

    char Read() {
        char Data = m_RxBuffer.back();
        m_RxBuffer.pop();
        return Data; 
    }

    bool IsEmpty() const {
        return m_RxBuffer.empty();
    }

    template<size_t N>
    void ReadData() {
        /*
        etl::string<N> Data;
        for (size_t i = 0; i < N && !m_RxBuffer.empty(); ++i) {
            Data.push_back(m_RxBuffer.back());
            m_RxBuffer.pop();
        }
        return Data;
        */
    }
};