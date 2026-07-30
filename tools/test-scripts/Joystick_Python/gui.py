import tkinter as tk
from tkinter import messagebox

import customtkinter as ctk

from serial_port import SerialController


class Joystick:
    def __init__(
        self,
        parent,
        title: str,
        horizontal_label: str,
        vertical_label: str,
        command,
        throttle_mode: bool = False
    ):
        self.command = command
        self.throttle_mode = throttle_mode

        self.size = 280
        self.center = self.size // 2
        self.radius = 100
        self.knob_radius = 18

        self.x_value = 0

        # Joystick ga ban đầu ở dưới cùng.
        if throttle_mode:
            self.y_value = -100
        else:
            self.y_value = 0

        self.frame = ctk.CTkFrame(parent)
        self.frame.pack(
            side="left",
            padx=15,
            pady=15,
            expand=True
        )

        title_widget = ctk.CTkLabel(
            self.frame,
            text=title,
            font=("Arial", 19, "bold")
        )
        title_widget.pack(pady=(15, 5))

        self.canvas = tk.Canvas(
            self.frame,
            width=self.size,
            height=self.size,
            bg="#2b2b2b",
            highlightthickness=0
        )
        self.canvas.pack(padx=10, pady=10)

        self.canvas.create_oval(
            self.center - self.radius,
            self.center - self.radius,
            self.center + self.radius,
            self.center + self.radius,
            outline="white",
            width=3
        )

        self.canvas.create_line(
            self.center - self.radius,
            self.center,
            self.center + self.radius,
            self.center,
            fill="gray",
            width=2
        )

        self.canvas.create_line(
            self.center,
            self.center - self.radius,
            self.center,
            self.center + self.radius,
            fill="gray",
            width=2
        )

        self.canvas.create_text(
            self.center,
            18,
            text=vertical_label,
            fill="white",
            font=("Arial", 13, "bold")
        )

        self.canvas.create_text(
            self.size - 30,
            self.center - 15,
            text=horizontal_label,
            fill="white",
            font=("Arial", 13, "bold")
        )

        initial_dy = self.radius if throttle_mode else 0

        self.knob = self.canvas.create_oval(
            self.center - self.knob_radius,
            self.center + initial_dy - self.knob_radius,
            self.center + self.knob_radius,
            self.center + initial_dy + self.knob_radius,
            fill="#1f6aa5",
            outline="white",
            width=2
        )

        self.value_label = ctk.CTkLabel(
            self.frame,
            text=f"X: {self.x_value}   Y: {self.y_value}",
            font=("Arial", 14)
        )
        self.value_label.pack(pady=(0, 15))

        self.canvas.bind("<Button-1>", self.move)
        self.canvas.bind("<B1-Motion>", self.move)
        self.canvas.bind("<ButtonRelease-1>", self.release)

    def move(self, event):
        dx = event.x - self.center
        dy = event.y - self.center

        distance = (dx * dx + dy * dy) ** 0.5

        if distance > self.radius:
            scale = self.radius / distance
            dx *= scale
            dy *= scale

        self.set_knob(dx, dy)

        self.x_value = int(dx * 100 / self.radius)

        # Đi lên là giá trị dương.
        self.y_value = int(-dy * 100 / self.radius)

        self.update_label()
        self.command(self.x_value, self.y_value)

    def release(self, _event):
        if self.throttle_mode:
            # Joystick trái:
            # Yaw trở về giữa, throttle giữ nguyên.
            dy = -self.y_value * self.radius / 100
            self.x_value = 0
            self.set_knob(0, dy)

        else:
            # Joystick phải:
            # Roll và pitch đều trở về giữa.
            self.x_value = 0
            self.y_value = 0
            self.set_knob(0, 0)

        self.update_label()
        self.command(self.x_value, self.y_value)

    def set_knob(self, dx, dy):
        x = self.center + dx
        y = self.center + dy

        self.canvas.coords(
            self.knob,
            x - self.knob_radius,
            y - self.knob_radius,
            x + self.knob_radius,
            y + self.knob_radius
        )

    def update_label(self):
        self.value_label.configure(
            text=f"X: {self.x_value}   Y: {self.y_value}"
        )

    def reset(self):
        self.x_value = 0

        if self.throttle_mode:
            self.y_value = -100
            self.set_knob(0, self.radius)
        else:
            self.y_value = 0
            self.set_knob(0, 0)

        self.update_label()


class DroneControllerApp(ctk.CTk):
    def __init__(self):
        super().__init__()

        self.serial_controller = SerialController()

        self.throttle = 1000
        self.yaw = 1500
        self.pitch = 1500
        self.roll = 1500

        self.title("STM32 Drone Controller")
        self.geometry("850x690")
        self.resizable(False, False)

        ctk.set_appearance_mode("dark")
        ctk.set_default_color_theme("blue")

        self.create_widgets()
        self.refresh_ports()

        self.protocol("WM_DELETE_WINDOW", self.close_app)

    def create_widgets(self):
        title = ctk.CTkLabel(
            self,
            text="STM32 Drone Joystick Controller",
            font=("Arial", 27, "bold")
        )
        title.pack(pady=(20, 10))

        connection_frame = ctk.CTkFrame(self)
        connection_frame.pack(
            padx=30,
            pady=10,
            fill="x"
        )

        self.port_menu = ctk.CTkOptionMenu(
            connection_frame,
            values=["Không tìm thấy"],
            width=170
        )
        self.port_menu.pack(
            side="left",
            padx=15,
            pady=15
        )

        refresh_button = ctk.CTkButton(
            connection_frame,
            text="Quét lại",
            command=self.refresh_ports
        )
        refresh_button.pack(side="left", padx=10)

        self.connect_button = ctk.CTkButton(
            connection_frame,
            text="Kết nối",
            command=self.toggle_connection
        )
        self.connect_button.pack(side="left", padx=10)

        self.status_label = ctk.CTkLabel(
            connection_frame,
            text="Chưa kết nối",
            text_color="red",
            font=("Arial", 15)
        )
        self.status_label.pack(side="left", padx=20)

        joystick_frame = ctk.CTkFrame(self)
        joystick_frame.pack(
            padx=30,
            pady=15,
            fill="both",
            expand=True
        )

        self.left_joystick = Joystick(
            joystick_frame,
            title="JOYSTICK TRÁI",
            horizontal_label="YAW",
            vertical_label="THROTTLE",
            command=self.left_changed,
            throttle_mode=True
        )

        self.right_joystick = Joystick(
            joystick_frame,
            title="JOYSTICK PHẢI",
            horizontal_label="ROLL",
            vertical_label="PITCH",
            command=self.right_changed,
            throttle_mode=False
        )

        self.data_label = ctk.CTkLabel(
            self,
            text=(
                "Throttle: 1000 | "
                "Yaw: 1500 | "
                "Pitch: 1500 | "
                "Roll: 1500"
            ),
            font=("Arial", 16, "bold")
        )
        self.data_label.pack(pady=8)

        stop_button = ctk.CTkButton(
            self,
            text="DỪNG KHẨN CẤP",
            width=230,
            font=("Arial", 17, "bold"),
            fg_color="red",
            hover_color="darkred",
            command=self.emergency_stop
        )
        stop_button.pack(pady=(5, 20))

    def refresh_ports(self):
        ports = self.serial_controller.get_ports()

        if ports:
            self.port_menu.configure(values=ports)

            if "COM3" in ports:
                self.port_menu.set("COM3")
            else:
                self.port_menu.set(ports[0])
        else:
            self.port_menu.configure(
                values=["Không tìm thấy"]
            )
            self.port_menu.set("Không tìm thấy")

    def toggle_connection(self):
        if self.serial_controller.is_connected():
            self.serial_controller.disconnect()

            self.status_label.configure(
                text="Đã ngắt kết nối",
                text_color="red"
            )
            self.connect_button.configure(text="Kết nối")
            return

        port = self.port_menu.get()

        if port == "Không tìm thấy":
            messagebox.showwarning(
                "Không tìm thấy COM",
                "Hãy cắm CP2102 rồi nhấn Quét lại."
            )
            return

        try:
            self.serial_controller.connect(
                port=port,
                baudrate=9600
            )

            self.status_label.configure(
                text=f"Đã kết nối {port}",
                text_color="green"
            )
            self.connect_button.configure(
                text="Ngắt kết nối"
            )

            self.send_control_data()

        except Exception as error:
            messagebox.showerror(
                "Lỗi kết nối",
                str(error)
            )

    def left_changed(self, x_value, y_value):
        self.yaw = self.map_value(
            x_value,
            -100,
            100,
            1000,
            2000
        )

        self.throttle = self.map_value(
            y_value,
            -100,
            100,
            1000,
            1950
        )

        self.update_display()
        self.send_control_data()

    def right_changed(self, x_value, y_value):
        self.roll = self.map_value(
            x_value,
            -100,
            100,
            1000,
            2000
        )

        self.pitch = self.map_value(
            y_value,
            -100,
            100,
            1000,
            2000
        )

        self.update_display()
        self.send_control_data()

    def send_control_data(self):
        if not self.serial_controller.is_connected():
            return

        try:
            self.serial_controller.send_control(
                throttle=self.throttle,
                yaw=self.yaw,
                pitch=self.pitch,
                roll=self.roll
            )

        except Exception as error:
            messagebox.showerror(
                "Lỗi gửi UART",
                str(error)
            )

    def update_display(self):
        self.data_label.configure(
            text=(
                f"Throttle: {self.throttle} | "
                f"Yaw: {self.yaw} | "
                f"Pitch: {self.pitch} | "
                f"Roll: {self.roll}"
            )
        )

    def emergency_stop(self):
        self.throttle = 1000
        self.yaw = 1500
        self.pitch = 1500
        self.roll = 1500

        self.left_joystick.reset()
        self.right_joystick.reset()

        self.update_display()
        self.send_control_data()

    def close_app(self):
        try:
            if self.serial_controller.is_connected():
                self.throttle = 1000
                self.yaw = 1500
                self.pitch = 1500
                self.roll = 1500

                self.send_control_data()
                self.serial_controller.disconnect()

        finally:
            self.destroy()

    @staticmethod
    def map_value(
        value,
        input_min,
        input_max,
        output_min,
        output_max
    ):
        result = (
            (value - input_min)
            * (output_max - output_min)
            / (input_max - input_min)
            + output_min
        )

        return int(result)