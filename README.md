# STM32 Autonomous Quadcopter Flight Controller

Firmware flight controller tự phát triển cho drone quadcopter F450/S500,
được lập trình bằng STM32CubeIDE.

Mục tiêu dài hạn của dự án là xây dựng một flight controller có khả năng:

- đọc và xử lý dữ liệu cảm biến;
- ước lượng attitude và altitude;
- điều khiển rate, attitude và altitude bằng PID cascade;
- giao tiếp với Raspberry Pi để thực hiện autonomous flight;
- giao tiếp MAVLink với ground station hoặc ứng dụng tự phát triển.

> Dự án đang trong giai đoạn phát triển và chưa được chứng nhận an toàn bay.

## Hardware target

### MCU

Reference MCU hiện tại:

- STM32F411CEU6

MCU dự kiến cho phiên bản có khả năng mở rộng tốt hơn:

- STM32F405RGT6

### Sensors

- ICM42688P — 6-axis IMU, giao tiếp SPI
- BMP280 — barometer và temperature sensor, giao tiếp I2C
- GY-271 — magnetometer/compass, giao tiếp I2C

### Actuator

- 4 ESC outputs
- PWM motor control
- Quad-X frame

### Airframe

- F450
- S500

## Current project status

### Đã hoàn thành

- [x] Đọc ICM42688P qua SPI
- [x] Đọc BMP280 qua I2C
- [x] Đọc GY-271 qua I2C
- [x] Xuất PWM cho bốn ESC
- [x] Kiểm tra motor spin khi tháo cánh
- [x] Thiết lập repository chung
- [x] Thiết lập GitHub Flow
- [x] Branch protection và Pull Request review
- [x] CODEOWNERS và Issue Forms

### Đang phát triển

- [ ] Chuẩn hóa sensor interface
- [ ] Timestamp cho sensor sample
- [ ] Gyroscope bias calibration
- [ ] Accelerometer calibration
- [ ] Magnetometer calibration
- [ ] Sensor filtering
- [ ] Mahony attitude estimator
- [ ] Angular-rate PID
- [ ] Attitude controller
- [ ] Altitude estimator
- [ ] Altitude hold
- [ ] Position control
- [ ] Arming và failsafe
- [ ] Flight-data logging
- [ ] MAVLink communication
- [ ] Raspberry Pi companion link

## Repository structure

```text
.
├── .github/
│   ├── ISSUE_TEMPLATE/
│   ├── CODEOWNERS
│   └── pull_request_template.md
├── docs/
│   ├── architecture/
│   ├── bringup/
│   ├── repository/
│   ├── testing/
│   └── coordinate-frames.md
├── firmware/
│   └── flight-controller/
│       ├── App/
│       ├── Core/
│       ├── Drivers/
│       ├── Platform/
│       ├── Sensors/
│       ├── flight-controller.ioc
│       └── STM32F411CEUX_FLASH.ld
├── hardware/
│   ├── pinout/
│   ├── schematics/
│   └── wiring/
├── third_party/
├── tools/
│   ├── calibration/
│   ├── log-parser/
│   └── test-scripts/
├── CONTRIBUTING.md
└── README.md
```

## Development environment

Project được phát triển bằng:

- STM32CubeIDE
- STM32CubeMX
- GNU Arm Embedded Toolchain
- ST-Link programmer/debugger
- Git
- GitHub

Phiên bản STM32CubeIDE chính thức của team được ghi tại:

```text
docs/bringup/development-setup.md
```

Tất cả thành viên nên sử dụng cùng:

- STM32CubeIDE version;
- STM32CubeF4 firmware package;
- compiler version;
- project configuration.

## Clone repository

```bash
git clone https://github.com/ZzzDolphiinn52Hz/autopilot_myself.git
cd autopilot_myself
```

## Import project vào STM32CubeIDE

1. Mở STM32CubeIDE.
2. Chọn `File`.
3. Chọn `Import`.
4. Chọn `Existing Projects into Workspace`.
5. Chọn thư mục:

```text
firmware/flight-controller
```

6. Xác nhận project được nhận diện.
7. Chọn `Finish`.

## Build firmware

Trong STM32CubeIDE:

1. Chọn project `flight-controller`.
2. Chọn cấu hình `Debug`.
3. Chọn `Project > Build Project`.
4. Kiểm tra không có compiler error.
5. Không bổ sung warning mới mà không giải thích trong Pull Request.

Build output nằm trong:

```text
firmware/flight-controller/Debug/
```

Thư mục này không được commit vào Git.

## Flash firmware

1. Kết nối ST-Link với board.
2. Cấp nguồn phù hợp cho board.
3. Chọn cấu hình debug trong STM32CubeIDE.
4. Build project.
5. Chọn `Run` hoặc `Debug`.

Trước khi flash, xác nhận:

- đúng MCU target;
- đúng linker script;
- đúng file `.ioc`;
- đúng pin mapping;
- không gắn cánh quạt trong giai đoạn bring-up.

## Git workflow

Project sử dụng GitHub Flow:

```text
Issue
  ↓
Branch từ main
  ↓
Code và test
  ↓
Commit
  ↓
Push branch
  ↓
Pull Request
  ↓
Code Owner review
  ↓
Squash merge
```

Không push trực tiếp vào `main`.

Ví dụ tạo branch:

```bash
git switch main
git pull --ff-only origin main
git switch -c feat/mahony-estimator
```

Commit:

```bash
git add <changed-files>
git commit -m "feat(estimator): add Mahony quaternion update"
```

Push:

```bash
git push -u origin feat/mahony-estimator
```

Chi tiết xem tại:

- `CONTRIBUTING.md`
- `docs/repository/code-review-policy.md`

## Commit convention

Format:

```text
<type>(<scope>): <description>
```

Ví dụ:

```text
feat(imu): add ICM42688 data-ready interrupt
fix(baro): correct BMP280 pressure compensation
feat(estimator): add Mahony attitude estimator
fix(mixer): correct rear-right motor yaw sign
docs(repo): update development workflow
build(board): configure SPI1 DMA
```

Các type thường dùng:

- `feat`
- `fix`
- `refactor`
- `test`
- `docs`
- `build`
- `ci`
- `chore`
- `perf`

## Pull Request rules

Mỗi Pull Request phải:

- giải quyết một mục tiêu chính;
- sử dụng Pull Request template;
- có Code Owner review;
- không chứa build artifact;
- mô tả thay đổi `.ioc`, timer, DMA và interrupt;
- cung cấp bằng chứng test khi thay đổi firmware;
- resolve toàn bộ conversation trước khi merge;
- sử dụng Squash merge.

## Safety rules

Luôn tháo cánh quạt khi thực hiện:

- sensor bring-up;
- kiểm tra motor order;
- kiểm tra motor direction;
- PWM test;
- mixer test;
- PID bench test;
- interrupt và timing test;
- failsafe test ban đầu.

Không thực hiện flight test khi:

- sensor axis chưa được xác nhận;
- motor order chưa đúng;
- motor direction chưa đúng;
- estimator chưa ổn định;
- arm/disarm chưa hoạt động;
- chưa có emergency stop;
- có warning hoặc lỗi timing chưa xác định;
- chưa có người giám sát khu vực test.

## Documentation

Tài liệu chính được lưu trong:

```text
docs/
```

Các nội dung quan trọng:

```text
docs/bringup/development-setup.md
docs/coordinate-frames.md
docs/repository/code-review-policy.md
```

Mọi thay đổi về hardware, pin, timer, DMA hoặc sensor orientation phải được cập
nhật vào tài liệu tương ứng.

## License

License của project chưa được chốt.

Không copy source code từ project khác vào repository trước khi kiểm tra license
tương ứng.

Các project tham khảo có thể sử dụng license khác nhau, ví dụ BSD hoặc GPL.