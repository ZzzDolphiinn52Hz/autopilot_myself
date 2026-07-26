# Pull Request

## Mục tiêu

<!--
Mô tả vấn đề mà Pull Request này giải quyết.
Không chỉ mô tả rằng bạn đã sửa file nào.
-->

Closes #

## Thay đổi chính

- 
- 
- 

## Phạm vi ảnh hưởng

Đánh dấu các khu vực bị ảnh hưởng:

- [ ] Chỉ thay đổi tài liệu
- [ ] Cấu hình repository hoặc CI
- [ ] Board support hoặc STM32CubeIDE
- [ ] File CubeMX `.ioc`
- [ ] Sensor driver
- [ ] Sensor calibration hoặc filtering
- [ ] Estimator
- [ ] PID hoặc controller
- [ ] Mixer hoặc motor output
- [ ] Communication, UART hoặc MAVLink
- [ ] Arming, failsafe hoặc safety
- [ ] Tool, script hoặc log analysis

## Build

- [ ] Build thành công trên STM32CubeIDE
- [ ] Không phát sinh compiler error
- [ ] Không phát sinh warning mới
- [ ] Không commit thư mục `Debug`, `Release` hoặc build artifact
- [ ] Không áp dụng vì Pull Request chỉ thay đổi tài liệu

### Build target

```text
MCU:
STM32CubeIDE version:
Build configuration: Debug / Release
```

---
TEMPLATE KHÁC
---
## Hardware test

- [ ] Đã test trên hardware
- [ ] Đã thực hiện test ban đầu khi tháo cánh
- [ ] Sensor đọc đúng dữ liệu
- [ ] Sensor axis và dấu dữ liệu đúng
- [ ] Motor order và motor direction đúng
- [ ] Failsafe liên quan vẫn hoạt động
- [ ] Không áp dụng cho Pull Request này

**Mô tả quy trình và kết quả test:**
- **Board:**
- **Thiết bị được kết nối:**
- **Các bước test:**
- **Kết quả:**

## CubeMX, timer, DMA và interrupt

- [ ] Không thay đổi file `.ioc`
- [ ] Có thay đổi file `.ioc`
- [ ] Không thay đổi pin mapping
- [ ] Không thay đổi timer hoặc PWM
- [ ] Không thay đổi DMA
- [ ] Không thay đổi interrupt priority

**Nếu có thay đổi, mô tả chi tiết:**
- **Peripheral:**
- **Pin:**
- **Timer/channel:**
- **DMA stream:**
- **Interrupt:**
- **Lý do thay đổi:**

## Kiểm tra flight-critical code

*Áp dụng khi Pull Request ảnh hưởng estimator, controller, mixer, motor hoặc safety:*

- [ ] Không dùng `HAL_Delay()` trong control path
- [ ] Không thêm thao tác blocking trong ISR
- [ ] Không cấp phát bộ nhớ động trong flight-critical path
- [ ] Đã kiểm tra giới hạn input và output
- [ ] Đã kiểm tra trường hợp sensor invalid hoặc timeout
- [ ] Đã kiểm tra actuator saturation
- [ ] Đã kiểm tra reset hoặc giới hạn PID integral
- [ ] Không áp dụng cho Pull Request này

## Bằng chứng kiểm tra

*Đính kèm một hoặc nhiều nội dung phù hợp:*

- [ ] Serial output
- [ ] Build log
- [ ] Sensor log
- [ ] Plot
- [ ] Ảnh bench test
- [ ] Video motor test khi tháo cánh

## Rủi ro và phương án quay lại

**Rủi ro có thể xảy ra:**
- (Mô tả rủi ro...)

**Cách quay lại phiên bản trước nếu có lỗi:**
- (Mô tả rollback...)

## Checklist trước khi yêu cầu review

- [ ] Pull Request chỉ giải quyết một mục tiêu chính
- [ ] Branch được tạo từ nhánh `main` mới nhất
- [ ] Đã tự kiểm tra phần **Files changed**
- [ ] Không có file không liên quan
- [ ] Không chứa mật khẩu, token hoặc thông tin bí mật
- [ ] Commit message tuân theo quy tắc: `<type>(<scope>): <description>`
- [ ] Đã chọn reviewer phù hợp

<!-- 
Lưu file bằng: Ctrl + S 
-->

