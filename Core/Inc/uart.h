#pragma once
#include <etl/circular_buffer.h>
#include <etl/string.h>
#include <etl/function.h>

class Uart final {
    using handle_t = __UART_HandleTypeDef;
    using return_t = HAL_StatusTypeDef;

    etl::circular_buffer<char,128> m_Buffer;
    handle_t*                      m_UartHandle;
    
    bool m_IsReady { false };
    bool m_HasData { false };
public:
    Uart() = default;
    Uart(handle_t* _Handle) : m_UartHandle{_Handle} 
    {
        // Empty
    }
    bool WaitForData(const char * _Data, size_t _TimeoutMs) 
    {
        int Size = strlen(_Data);

        if (ReceiveData(Size + 1, _TimeoutMs) == HAL_OK) 
        {
            if (etl::equal(_Data, _Data+Size ,m_Buffer.begin()))
            {
                return true;
            }
        }
        return false;
    }
    const auto& GetBuffer() const 
    {
        return m_Buffer; 
    }

    void HandleIRQ() {
        HAL_UART_StateTypeDef State = HAL_UART_GetState(m_UartHandle);   
        switch (State)
        {
            case HAL_UART_STATE_RESET: 
            {
                m_IsReady = false;
                break;
            }
            case HAL_UART_STATE_READY:
            {
                m_IsReady = true;
                break;
            }
            case HAL_UART_STATE_BUSY:
            break;
            case HAL_UART_STATE_BUSY_TX:
            break;
            case HAL_UART_STATE_BUSY_RX:
            break;
            case HAL_UART_STATE_BUSY_TX_RX:
            break;
            case HAL_UART_STATE_ERROR:
            break;
            default:
            break;
        }
        HAL_UART_IRQHandler(m_UartHandle);
    }
    return_t WriteDataAsync(const char * _Data, uint32_t _TimeoutMs = 1000) {
        return HAL_UART_Transmit(m_UartHandle, (uint8_t*)_Data, strlen(_Data) + 1, _TimeoutMs);
    }
    return_t WriteData(const char * _Data)
    {
        return WriteDataAsync(_Data);
    }
    
    return_t ReceiveData(int _Count, uint32_t _TimeoutMs = 1000)
    {
        uint8_t Buffer = '\0';
        for (int i = 0; i < _Count;++i) 
        {
            auto Status = HAL_UART_Receive(m_UartHandle, &Buffer, 1 ,_TimeoutMs);
            if (Status == HAL_OK)
                m_Buffer.push(Buffer);
            else
                return Status;
        }
        return HAL_OK;
    }
    return_t ReceiveDataAsync(size_t _Count)
    {
        return ReceiveData(_Count);
    }
    return_t StopReadWrite() const 
    {
        return HAL_UART_Abort(m_UartHandle);
    }
    return_t StopWriting() const 
    {
        return HAL_UART_AbortTransmit(m_UartHandle);
    }
    return_t StopReceiving() const
    {
        return HAL_UART_AbortReceive(m_UartHandle);
    }
};