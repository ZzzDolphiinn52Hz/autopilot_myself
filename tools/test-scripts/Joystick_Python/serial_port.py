import serial
import serial.tools.list_ports


class SerialController:
    def __init__(self):
        self.connection = None

    def get_ports(self) -> list[str]:
        return [
            port.device
            for port in serial.tools.list_ports.comports()
        ]

    def connect(self, port: str, baudrate: int = 9600) -> None:
        self.disconnect()

        self.connection = serial.Serial(
            port=port,
            baudrate=baudrate,
            timeout=1
        )

    def disconnect(self) -> None:
        if self.connection is not None:
            if self.connection.is_open:
                self.connection.close()

        self.connection = None

    def is_connected(self) -> bool:
        return (
            self.connection is not None
            and self.connection.is_open
        )

    def send_control(
        self,
        throttle: int,
        yaw: int,
        pitch: int,
        roll: int
    ) -> None:

        if not self.is_connected():
            raise RuntimeError("Chưa kết nối cổng COM")

        message = (
            f"{throttle},"
            f"{yaw},"
            f"{pitch},"
            f"{roll}\n"
        )

        self.connection.write(message.encode("ascii"))