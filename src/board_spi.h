#ifndef BOARD_SPI_H
#define BOARD_SPI_H

void board_spi_assert_cs(void);
void board_spi_deassert_cs(void);
uint8_t board_spi_transfer_byte(uint8_t data);
void board_configure_spi(void);

#endif // BOARD_SPI_H