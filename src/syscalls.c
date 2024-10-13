#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/times.h>

#include <stm32f4xx_usart.h>

// define UART channel for console
#define CONSOLE_UART    USART2

/**
 ******************************************************************************
 * @brief     Reentrant read
 * @param     *r   -- reentrant pointer
 * @param     file -- file descriptor
 * @param     ptr  -- file pointer
 * @param     len  -- length
 * @return    always 1
 * @attention Minimal implementation
 ******************************************************************************
 */
int _read_r (struct _reent *r, int file, char * ptr, int len)
{
    uint8_t ch;
    while (!USART_GetFlagStatus(CONSOLE_UART, USART_FLAG_RXNE)) {
        // wait Rx not Empty
    }
    ch = (uint8_t)USART_ReceiveData(CONSOLE_UART);            // UART 1byte recieve
    (void)USART_SendData(CONSOLE_UART, ch);                   // echo back
    *ptr = (char)ch;
    return 1;
}

/**
 ******************************************************************************
 * @brief     Reentrant lseek
 * @param     *r   -- reentrant pointer
 * @param     file -- file descriptor
 * @param     ptr  -- file pointer
 * @param     dir  -- seek direction
 * @return    always 0
 * @attention Minimal implementation
 ******************************************************************************
 */
int _lseek_r (struct _reent *r, int file, int ptr, int dir)
{
  return 0;
}

/**
 ******************************************************************************
 * @brief     Reentrant write
 * @param     *r   -- reentrant pointer
 * @param     file -- file descriptor
 * @param     ptr  -- file pointer
 * @param     len  -- length
 * @return    write length
 * @attention Minimal implementation
 ******************************************************************************
 */
int _write_r (struct _reent *r, int file, char * ptr, int len)
{  
  int index;
  
  /* For example, output string by UART */
  for(index=0; index<len; index++)
  {
    if (ptr[index] == '\n') {
        while (!USART_GetFlagStatus(CONSOLE_UART, USART_FLAG_TXE)) {
            // wait Tx Empty
        }
      // uart_write(SYS_UART_CH,'\r');
      USART_SendData(CONSOLE_UART, (uint8_t)'\r');
    }  
    while (!USART_GetFlagStatus(CONSOLE_UART, USART_FLAG_TXE)) {
        // wait Tx Empty
    }
    // UART 1byte send
    USART_SendData(CONSOLE_UART, (uint8_t)ptr[index]);
  }
  
  return len;
}

/**
 ******************************************************************************
 * @brief     Reentrant open
 * @param     *r   -- reentrant pointer
 * @param     *name -- file name
 * @param     flags -- file access permission
 * @param     mode  -- file mode
 * @return    always -1
 * @attention Minimal implementation
 ******************************************************************************
 */
int _open_r (struct _reent *r, const char *name, int flags, int mode)
{
  return -1;
}

/**
 ******************************************************************************
 * @brief     Reentrant close
 * @param     *r   -- reentrant pointer
 * @param     file -- file descriptor
 * @return    always 0
 * @attention Minimal implementation
 ******************************************************************************
 */
int _close_r (struct _reent *r, int file)
{
  return 0;
}

/* Register name faking - works in collusion with the linker.  */
register char * stack_ptr asm ("sp");

/**
 ******************************************************************************
 * @brief     Reentrant sbrk
 * @param     *r   -- reentrant pointer
 * @param     incr -- increase size
 * @return    heap end position
 * @attention Minimal implementation
 ******************************************************************************
 */
caddr_t _sbrk_r (struct _reent *r, int incr)
{
  extern char   end asm ("_HeapStart"); /* Defined by the linker.  */
  static char * heap_end;
  char *        prev_heap_end;

  if (heap_end == NULL)
    heap_end = & end;
  
  prev_heap_end = heap_end;
  
  if (heap_end + incr > stack_ptr)
  {
      /* Some of the libstdc++-v3 tests rely upon detecting
        out of memory errors, so do not abort here.  */
#if 0
      extern void abort (void);

      _write (1, "_sbrk: Heap and stack collision\n", 32);
      
      abort ();
#else
      errno = ENOMEM;
      return (caddr_t) -1;
#endif
  }
  
  heap_end += incr;

  return (caddr_t) prev_heap_end;
}

/**
 ******************************************************************************
 * @brief     Reentrant fstat
 * @param     *r   -- reentrant pointer
 * @param     file -- file descriptor
 * @param     *st  -- file status pointer
 * @return    always 0
 * @attention Minimal implementation
 ******************************************************************************
 */
int _fstat_r (struct _reent *r, int file, struct stat * st)
{
  memset (st, 0, sizeof (* st));
  st->st_mode = S_IFCHR;
  return 0;
}

/**
 ******************************************************************************
 * @brief     Reentrant isatty
 * @param     *r   -- reentrant pointer
 * @param     fd   -- file descriptor
 * @return    always 1
 * @attention Minimal implementation
 ******************************************************************************
 */
int _isatty_r(struct _reent *r, int fd)
{
  return 1;
}

/**
 ******************************************************************************
 * @brief     Reentrant times
 * @param     *r   -- reentrant pointer
 * @param     *t   -- timing information pointer
 * @return    Timing information
 * @attention Minimal implementation
 ******************************************************************************
 */
clock_t _times_r(struct _reent *r, struct tms * t)
{
  return -1;
}

/*** EOF ***/
