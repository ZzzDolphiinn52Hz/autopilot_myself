# Code Review Policy

Tài liệu này quy định mức review và bằng chứng kiểm tra cần thiết trước khi
một Pull Request được merge vào `main`.

## 1. Documentation changes

Áp dụng cho:

- README;
- hướng dẫn phát triển;
- tài liệu kiến trúc;
- tài liệu test;
- tài liệu hardware.

Yêu cầu:

- ít nhất một reviewer;
- kiểm tra Markdown;
- không có file không liên quan;
- không cần test hardware nếu chỉ thay đổi tài liệu.

## 2. Standard firmware changes

Áp dụng cho:

- sensor driver;
- communication;
- logging;
- calibration;
- utility module.

Yêu cầu:

- build thành công;
- không có warning mới;
- ít nhất một reviewer kỹ thuật;
- có mô tả hoặc bằng chứng bench test;
- không sử dụng thao tác blocking trong control path.

## 3. Flight-critical changes

Áp dụng cho:

- estimator;
- PID controller;
- motor mixer;
- actuator output;
- arming;
- failsafe;
- interrupt;
- DMA;
- timer;
- file CubeMX `.ioc`.

Yêu cầu:

- review bởi Code Owner phù hợp;
- build thành công;
- test ban đầu khi tháo cánh;
- cung cấp log, plot, ảnh hoặc video test;
- kiểm tra sensor axis và đơn vị;
- kiểm tra motor order và motor direction;
- kiểm tra timeout, saturation và failsafe liên quan.

## 4. CubeMX conflict policy

Chỉ một Pull Request được phép thay đổi file `.ioc` tại một thời điểm.

Pull Request thay đổi `.ioc` phải mô tả:

- pin mapping;
- peripheral;
- timer và channel;
- DMA stream;
- interrupt và priority;
- ảnh hưởng tới các module hiện tại.

Sau khi generate code từ CubeMX, tác giả phải kiểm tra `git diff` trước khi
commit để phát hiện các thay đổi ngoài dự kiến.

## 5. Merge policy

- Không push trực tiếp vào `main`.
- Mọi thay đổi phải đi qua Pull Request.
- Mọi conversation phải được resolve.
- Pull Request phải nhận đủ approval bắt buộc.
- Sử dụng Squash merge.
- Xóa branch sau khi merge.