# Development Setup

Tài liệu này hướng dẫn cách thiết lập môi trường phát triển (IDE, Compiler, Flashing tools) cho dự án Autopilot.

## 1. Toolchain & Dependencies

Dự án được phát triển và kiểm thử trên các phiên bản công cụ sau (khuyến nghị cài đặt đúng hoặc mới hơn):

*   **STM32CubeIDE:** v1.19.0
*   **STM32CubeMX:** v6.13.0 (Tích hợp sẵn trong CubeIDE hoặc cài rời)
*   **STM32Cube_FW_F4 Package:** v1.28.0 (Hoặc tương đương)
*   **Compiler:** GNU ARM Embedded Toolchain (arm-none-eabi-gcc) v13.3.1
*   **Board Target:** STM32F411CEU6 (WeAct Black Pill hoặc tương đương)
*   **ST-Link Version:** ST-Link V2 (hoặc V3)

---

## 2. Các Bước Import Project vào STM32CubeIDE

Do cấu trúc dự án là Monorepo (tách biệt phần cứng, công cụ và firmware), cần import đúng thư mục chứa source code C/C++:

1. Mở phần mềm **STM32CubeIDE**.
2. Chọn thư mục Workspace (bất kỳ thư mục nào, không nhất thiết phải là thư mục dự án).
3. Trên thanh Menu, chọn **File** -> **Import...**
4. Mở rộng mục **General** -> chọn **Existing Projects into Workspace** và ấn **Next**.
5. Ở phần *Select root directory*, ấn **Browse...** và trỏ tới thư mục `firmware/flight-controller/` bên trong thư mục gốc của repo.
6. Trong khung *Projects*, sẽ thấy project hiện ra (ví dụ: `driver_icm42688` hoặc `flight-controller`). Đảm bảo nó được tick.
7. Đảm bảo **KHÔNG** tick vào ô *"Copy projects into workspace"* (để code vẫn nằm trong thư mục git hiện tại).
8. Ấn **Finish**.

---

## 3. Các Bước Build Project

1. Ở khung **Project Explorer** (cột bên trái), click chuột trái chọn Project.
2. Trên thanh công cụ, ấn vào biểu tượng cái búa 🔨 (**Build**), hoặc dùng tổ hợp phím `Ctrl + B`.
3. Kiểm tra cửa sổ **Console** ở dưới cùng. Nếu quá trình thiết lập đúng, sẽ thấy thông báo:
   ```text
   Build Finished. 0 errors, 0 warnings.
   ```
*(Lưu ý: Một vài cảnh báo unused-variable có thể xuất hiện do các file driver chưa được gọi hết, điều này là bình thường).*

---

## 4. Các Bước Flash (Nạp Code) & Debug

1. Cắm mạch STM32F411 vào máy tính thông qua mạch nạp **ST-Link**.
2. Click chuột trái chọn Project trong **Project Explorer**.
3. Ấn vào biểu tượng con bọ 🐛 (**Debug**) trên thanh công cụ, hoặc chọn **Run -> Debug As -> STM32 Cortex-M C/C++ Application**.
4. Lần đầu tiên chạy, IDE sẽ hiện lên bảng cấu hình `Edit Configuration`. 
5. Chuyển sang tab **Debugger**, đảm bảo *Debug probe* đang chọn là `ST-LINK (ST-LINK GDB server)`.
6. Ấn **OK**. IDE sẽ biên dịch lại lần cuối (nếu cần) và nạp file `.elf` xuống mạch.
7. Khi nạp xong, chương trình sẽ tự động dừng tại dòng đầu tiên của hàm `main()`. Ấn nút **Resume** (biểu tượng ▶️ hoặc phím `F8`) để chương trình bắt đầu chạy.
