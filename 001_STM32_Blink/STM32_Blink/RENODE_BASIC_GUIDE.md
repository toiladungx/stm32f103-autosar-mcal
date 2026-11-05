# Hướng dẫn cơ bản về Renode cho STM32

## 1. Renode là gì?

Renode là một trình mô phỏng phần cứng mở rộng, dùng để chạy firmware trên môi trường ảo trước khi chạy thật trên board. Nó đặc biệt hữu ích cho:

- kiểm tra firmware STM32
- debug qua GDB
- mô phỏng GPIO, UART, timer, NVIC, EXTI
- test mà không cần board thật

Với STM32, Renode thường được dùng cùng với:

- file `.resc`: script điều khiển Renode
- file `.repl`: mô tả phần cứng và địa chỉ thanh ghi
- file ELF: firmware đã build

---

## 2. Cấu trúc file thường gặp

### 2.1 File script `.resc`

Ví dụ từ dự án:

```text
:name: STM32F103 Sample Debug
:description: Script chay Sample tren Renode

$elf_path ?= @build/FIRMWARE.elf
mach create "stm32"
machine LoadPlatformDescription @platform/debug/stm32f103_full.repl
sysbus LoadELF $elf_path
machine StartGdbServer 3333
```

File `.resc` dùng để:

- tạo máy mô phỏng
- nạp mô hình chip
- load firmware
- bật GDB server
- định nghĩa alias, button, LED,...

### 2.2 File mô tả phần cứng `.repl`

Ví dụ:

```text
usart1: UART.STM32_UART @ sysbus <0x40013800, +0x100>
    -> nvic@37

gpioPortA: GPIOPort.STM32F1GPIOPort @ sysbus <0x40010800, +0x400>
    [0-15] -> exti@[0-15]
```

File `.repl` mô tả:

- thanh ghi của chip
- địa chỉ bus
- kết nối với NVIC, EXTI, GPIO
- các peripheral như UART, GPIO, timer

---

## 3. Các lệnh cơ bản trong Renode

### 3.1 Khởi tạo máy mô phỏng

```text
mach create "stm32"
```

- tạo ra một machine tên `stm32`
- đây là bước bắt buộc trước khi nạp platform hay firmware

### 3.2 Tải mô tả platform

```text
machine LoadPlatformDescription @platform/debug/stm32f103_full.repl
```

- load file `.repl`
- tạo toàn bộ mô hình phần cứng STM32F103

### 3.3 Load firmware ELF

```text
sysbus LoadELF $elf_path
```

- nạp tệp ELF vào bộ nhớ hệ thống
- firmware sẽ chạy trên CPU mô phỏng

### 3.4 Hiển thị UART trên console

```text
showAnalyzer sysbus.usart1
```

- tự động hiển thị dữ liệu UART 1 trên terminal
- rất hữu ích với debug `printf`/serial log

### 3.5 Khởi động GDB Server

```text
machine StartGdbServer 3333
```

- mở GDB server trên cổng 3333
- dùng gdb remote connect để debug firmware

### 3.6 Thiết lập alias

```text
alias btn_down "sysbus.gpioPortA.userButtonPA0 Press"
alias btn_up   "sysbus.gpioPortA.userButtonPA0 Release"
alias led_check "sysbus.gpioPortC.ledPC13 State"
```

- rút gọn các lệnh dài
- tiện cho test GPIO, button, LED

### 3.7 Thiết lập log

```text
logLevel 3 sysbus
```

- bật mức log chi tiết của bus
- dùng để xem lời gọi bus, truy cập thanh ghi, lỗi phần cứng

### 3.8 In ra thông báo

```text
echo "Renode ready for GDB connection on port 3333. Use 'start' to begin simulation."
```

- hiển thị message khi script đã sẵn sàng

---

## 4. Các lệnh điều khiển chạy mô phỏng

### 4.1 Bắt đầu chạy

```text
start
```

- bắt đầu tiến trình mô phỏng CPU

### 4.2 Dừng mô phỏng

```text
stop
```

- dừng execution

### 4.3 Reset

```text
reset
```

- reset lại máy ảo và firmware

### 4.4 Thoát Renode

```text
quit
```

- đóng Renode

### 4.5 Trợ giúp

```text
help
```

- xem danh sách lệnh có sẵn

---

## 5. Cách tương tác với GPIO / Button / LED

### 5.1 Kích hoạt button

```text
sysbus.gpioPortA.userButtonPA0 Press
```

### 5.2 Thả button

```text
sysbus.gpioPortA.userButtonPA0 Release
```

### 5.3 Kiểm tra trạng thái LED

```text
sysbus.gpioPortC.ledPC13 State
```

Trong script mẫu, LED PC13 được định nghĩa với `invert: true`, tức nghĩa là:

- logic 0 = LED sáng
- logic 1 = LED tắt

Đây là xu hướng rất thường gặp trên STM32 vì chân LED active low

---

## 6. Ý nghĩa của EXTI và NVIC trong file `.repl`

Ví dụ:

```text
exti: IRQControllers.STM32F4_EXTI @ sysbus 0x40010400
    numberOfOutputLines: 19
    [0-4] -> nvic@[6-10]
    [5-9] -> nvicInput23@[0-4]
    [10-15] -> nvicInput40@[0-5]
```

Giải thích ngắn gọn:

- `exti` là bộ xử lý ngắt ngoại vi của STM32
- `numberOfOutputLines: 19` nghĩa có 19 đường ngắt ra
- `[0-4] -> nvic@[6-10]`: các line 0 tới 4 được nối tới NVIC IRQ 6..10
- `[5-9] -> nvicInput23@[0-4]`: các line 5..9 gộp vào một đầu vào chung rồi nối vào IRQ 23
- `[10-15] -> nvicInput40@[0-5]`: các line 10..15 gộp vào đầu vào khác rồi nối vào IRQ 40

Nghĩa là:

- GPIO có thể sinh interrupt
- EXTI chuyển tín hiệu từ chân GPIO sang NVIC
- CPU mô phỏng sẽ biết khi nào phải xử lý interrupt

---

## 7. Cách chạy Renode với script

### 7.1 Chạy script từ dòng lệnh

```bash
renode --console scripts/stm32_sample.resc
```

Nếu dùng GUI:

```bash
renode scripts/stm32_sample.resc
```

### 7.2 Chạy script với GDB connect

Sau khi script chạy:

```bash
gdb-multiarch build/FIRMWARE.elf
(gdb) target remote :3333
```

Sau đó bạn có thể:

- `continue`
- `break main`
- `step`
- `info registers`
- `x/10i $pc`

---

## 8. Một số ví dụ lệnh hữu ích

```text
showAnalyzer sysbus.usart1
logLevel 3 sysbus
sysbus.gpioPortA.userButtonPA0 Press
sysbus.gpioPortA.userButtonPA0 Release
sysbus.gpioPortC.ledPC13 State
machine StartGdbServer 3333
start
reset
quit
```

---

## 9. Mẹo khi làm việc với STM32 trên Renode

1. Luôn kiểm tra file `.repl` đúng với chip đang dùng.
2. Load đúng file ELF đã build xong.
3. Nếu dùng UART, bật `showAnalyzer` để nhìn log.
4. Với GPIO, nhớ kiểm tra chân là active low hay active high.
5. Nếu firmware không chạy, kiểm tra:
   - địa chỉ flash/sram
   - đúng platform file
   - đúng ELF
   - GDB port không bị conflict

---

## 10. Kết luận

Renode không chỉ là công cụ mô phỏng, mà còn là môi trường debug mạnh cho firmware STM32. Với các lệnh cơ bản như:

- `mach create`
- `machine LoadPlatformDescription`
- `sysbus LoadELF`
- `showAnalyzer`
- `machine StartGdbServer`
- `start`
- `reset`

bạn đã có nền tảng để chạy và debug firmware như trên board thật.

---

## 11. Tài liệu tham khảo nhanh

- `.resc` = script Renode
- `.repl` = mô tả phần cứng
- `ELF` = firmware binary
- `UART` = console output
- `GPIO` = chân input/output
- `EXTI` = external interrupt controller
- `NVIC` = nested vector interrupt controller

Nếu bạn muốn, mình có thể tiếp tục tạo thêm một file `RENODE_DEBUG_WORKFLOW.md` hướng dẫn từng bước debug firmware STM32 bằng Renode + GDB. 
