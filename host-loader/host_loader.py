import serial
import sys
import binascii

def main():
	ser = serial.Serial(sys.argv[1], baudrate=115200, timeout=0.5)
	f = open(sys.argv[2], 'rb')
	fw = f.read()
	size = len(fw)
	crc = binascii.crc32(fw)

	print('FW size: ' + repr(size) + ', CRC: ' + repr(crc))

	# write size LE
	ser.write(bytes([(size & 0xff), ((size >> 8) & 0xff), ((size >> 16) & 0xff)]))

	# write CRC32 LE
	ser.write(bytes([(crc & 0xff), ((crc >> 8) & 0xff), ((crc >> 16) & 0xff), ((crc >> 24) & 0xff)]))

	ser.write(fw)

	print("Done sending MCU fw")

if __name__ == '__main__':
	sys.exit(main())
