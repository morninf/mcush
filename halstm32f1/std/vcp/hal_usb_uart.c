#include "mcush.h"
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"


extern uint8_t  USART_Rx_Buffer[USART_RX_DATA_SIZE]; 
extern uint32_t USART_Rx_ptr_in;
extern uint32_t USART_Rx_ptr_out;
extern uint32_t USART_Rx_length;


#define QUEUE_VCP_RX_LEN    128
#define QUEUE_VCP_TX_LEN    128

QueueHandle_t hal_queue_vcp_rx;
QueueHandle_t hal_queue_vcp_tx;

signed portBASE_TYPE hal_vcp_putc( char c, TickType_t xBlockTime )
{

    if( xQueueSend( hal_queue_vcp_tx, &c, xBlockTime ) == pdPASS )
    {
        return pdPASS;
    }
    else
        return pdFAIL;
}

signed portBASE_TYPE hal_vcp_getc( char *c, TickType_t xBlockTime )
{
    return xQueueReceive( hal_queue_vcp_rx, c, xBlockTime );
}


void hal_vcp_reset(void)
{
    xQueueReset( hal_queue_vcp_rx );
    xQueueReset( hal_queue_vcp_tx );
}


void hal_vcp_enable(uint8_t enable)
{
}


int hal_uart_init(uint32_t baudrate)
{
    hal_queue_vcp_rx = xQueueCreate( QUEUE_VCP_RX_LEN, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
    hal_queue_vcp_tx = xQueueCreate( QUEUE_VCP_TX_LEN, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
    if( !hal_queue_vcp_rx || !hal_queue_vcp_tx )
        return 0;

    Get_SerialNum();  /* Serial Number from Chip UID */
    Set_System();
    Set_USBClock();
    USB_Interrupts_Config();
    USB_Init();
    return 1;
}


int  shell_driver_init( void )
{
    return 1;  /* already inited */
}


void shell_driver_reset( void )
{
    hal_vcp_reset();
}


int  shell_driver_read( char *buffer, int len )
{
    return 0;  /* not supported */
}


int  shell_driver_read_char( char *c )
{
    if( hal_vcp_getc( c, portMAX_DELAY ) == pdFAIL )
        return -1;
    else
        return (int)c;
}


int  shell_driver_read_char_blocked( char *c, int block_time )
{
    if( hal_vcp_getc( c, block_time ) == pdFAIL )
        return -1;
    else
        return (int)c;
}


int  shell_driver_read_is_empty( void )
{
    return 1;
}

#define WRITE_RETRY         5
#define WRITE_TIMEOUT_MS    1000
#define WRITE_TIMEOUT_TICK  (WRITE_TIMEOUT_MS*configTICK_RATE_HZ/1000)

int  shell_driver_write( const char *buffer, int len )
{
    int written=0;
    int retry;

    while( written < len )
    {
        retry = 0;
        while( hal_vcp_putc( *(char*)((int)buffer + written), WRITE_TIMEOUT_TICK ) == pdFAIL )
        {
            vTaskDelay(1);
            retry++;
            if( retry >= WRITE_RETRY )
                return written;
        }
        written += 1;
    }
    return written;
}


void shell_driver_write_char( char c )
{
    int retry=0;
    
    while( hal_vcp_putc( c, WRITE_TIMEOUT_TICK ) == pdFAIL )
    {
        vTaskDelay(1);
        retry++;
        if( retry >= WRITE_RETRY )
            return;
    }
}

