import serial

def send_to_nir(Bytes, command):
    ser = serial.Serial(
        port='/dev/ttyUSB0',
        baudrate=115200,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS
    )
    ser.write(Bytes)
    data = ser.read_until(b"\x44\x43\x42\x41")
    return(data)

def empacota(flags, sequence, command, data):
    cksumMsg = b''
    UARTHeader = b"\x41\x42\x43\x44"
    UARTTrailer = b"\x44\x43\x42\x41"
    data_len = len(data) + 2
    cksumMsg += bytes(flags + sequence) + data_len.to_bytes(2, byteorder='little') + bytes(command + data)
    checksum_int = (sum(cksumMsg))
    checksum = checksum_int.to_bytes(4, "little")
    package_data = UARTHeader + checksum + cksumMsg + UARTTrailer
    return(package_data)

def array_touint32(data):
    return data[0] + (data[1] << 8) + (data[2] << 16) + (data[3] << 24)   #Inverte os dados de little endian para big endian


def  NNO_CMD_READ_TEMP():
    flags = [0b11000000]
    sequence = [0]
    command = [0x00, 0x03]
    data = []

    temp_ans = send_to_nir(empacota(flags, sequence, command, data), command)
    data_index_start = temp_ans.index(bytes(command)) + 2
    data_index_end = temp_ans.index(b"\x44\x43\x42\x41")
    temp_data = temp_ans[data_index_start:data_index_end]
    ambient_temp = array_touint32(temp_data[:4])/100
    detect_temp = array_touint32(temp_data[-4:])/100
    print("temperatura detector = {} C".format(detect_temp))
    print("temperatura ambiente = {} C".format(ambient_temp))


def NNO_CMD_READ_HUM():
    flags = [0b11000000]
    sequence = [0]
    command = [0x02, 0x03]
    data = []
    crude = send_to_nir(empacota(flags, sequence, command, data), command)
    data_index_start = crude.index(bytes(command)) + 2
    data_index_end = crude.index(b"\x44\x43\x42\x41")
    humity_data = crude[data_index_start:data_index_end]
    humity = array_touint32(humity_data[-4:])/100
    hum_sensor_temp = array_touint32(humity_data[:4])/100
    print("Humidade = {} %".format(humity))
    print("Temperatura sensor humidade = {} C".format(hum_sensor_temp))

if __name__ == "__main__":
    NNO_CMD_READ_TEMP()
    NNO_CMD_READ_HUM()