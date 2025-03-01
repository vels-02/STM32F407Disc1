#include <stdint.h>



// RCC Registers (Base address: 0x40023800)

#define RCC_BASE           0x40023800

#define RCC_AHB1ENR        *((volatile uint32_t*)(RCC_BASE + 0x30))

#define RCC_APB1ENR        *((volatile uint32_t*)(RCC_BASE + 0x40))



// GPIOB Registers (Base address: 0x40020400)

#define GPIOB_BASE         0x40020400

#define GPIOB_MODER        *((volatile uint32_t*)(GPIOB_BASE + 0x00))

#define GPIOB_AFRH         *((volatile uint32_t*)(GPIOB_BASE + 0x24))

#define GPIOB_OSPEEDR      *((volatile uint32_t*)(GPIOB_BASE + 0x08))

#define GPIOB_BSRR         *((volatile uint32_t*)(GPIOB_BASE + 0x18))



// SPI2 Registers (Base address: 0x40003800)

#define SPI2_BASE          0x40003800

#define SPI2_CR1           *((volatile uint32_t*)(SPI2_BASE + 0x00))

#define SPI2_CR2           *((volatile uint32_t*)(SPI2_BASE + 0x04))

#define SPI2_SR            *((volatile uint32_t*)(SPI2_BASE + 0x08))

#define SPI2_DR            *((volatile uint8_t*)(SPI2_BASE + 0x0C)) // 8-bit access



// Bit positions

#define RCC_AHB1ENR_GPIOBEN   (1 << 1)

#define RCC_APB1ENR_SPI2EN    (1 << 14)



#define SPI_CR1_SPE           (1 << 6)

#define SPI_CR1_MSTR          (1 << 2)

#define SPI_CR1_SSM           (1 << 9)

#define SPI_CR1_SSI           (1 << 8)

#define SPI_CR1_BR_DIV16      (3 << 3) // Baud rate: PCLK/16



#define SPI_SR_TXE            (1 << 1)

#define SPI_SR_RXNE           (1 << 0)



#define CS_LOW()              (GPIOB_BSRR = (1 << 12) << 16) // Reset PB12

#define CS_HIGH()             (GPIOB_BSRR = (1 << 12))       // Set PB12



// Function prototypes

void SPI2_Init(void);

uint8_t SPI2_TransmitReceive(uint8_t data);

uint8_t BMP280_ReadRegister(uint8_t reg);



int main(void) {

    SPI2_Init();



    // Read BMP280 chip ID

    uint8_t chip_id = BMP280_ReadRegister(0xD0);



    while (1);

}



void SPI2_Init(void) {

    // Enable clocks for GPIOB and SPI2

    RCC_AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    RCC_APB1ENR |= RCC_APB1ENR_SPI2EN;



    // Configure GPIOB pins: PB13 (SCK), PB14 (MISO), PB15 (MOSI) as Alternate Function (AF5)

    GPIOB_MODER &= ~((3 << 26) | (3 << 28) | (3 << 30)); // Clear bits

    GPIOB_MODER |= (2 << 26) | (2 << 28) | (2 << 30);   // Set AF mode



    // Set alternate function AF5 for SPI2

    GPIOB_AFRH |= (5 << 20) | (5 << 24) | (5 << 28);



    // Configure PB12 (CS) as general-purpose output

    GPIOB_MODER &= ~(3 << 24); // Clear bits

    GPIOB_MODER |= (1 << 24);  // Output mode



    // Set high speed for SPI2 pins

    GPIOB_OSPEEDR |= (3 << 26) | (3 << 28) | (3 << 30);



    // Set CS high initially

    CS_HIGH();



    // Configure SPI2: Master, software slave management, clock div16, CPOL=0, CPHA=0, 8-bit data

    SPI2_CR1 = SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_BR_DIV16;



    // Enable SPI2

    SPI2_CR1 |= SPI_CR1_SPE;

}



uint8_t SPI2_TransmitReceive(uint8_t data) {

    // Wait until TX buffer is empty

    while (!(SPI2_SR & SPI_SR_TXE));



    // Send data

    SPI2_DR = data;



    // Wait until RX buffer is not empty

    while (!(SPI2_SR & SPI_SR_RXNE));



    // Read and return received data

    return SPI2_DR;

}



uint8_t BMP280_ReadRegister(uint8_t reg) {

    uint8_t value;



    CS_LOW();

    value = SPI2_TransmitReceive(reg | 0x80); // Set MSB high for read

    value = SPI2_TransmitReceive(0xFF); // Dummy byte to clock in data

    CS_HIGH();



    return value;

}
