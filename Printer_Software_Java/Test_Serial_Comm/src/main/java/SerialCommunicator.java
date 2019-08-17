import com.fazecast.jSerialComm.*;

import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;

public class SerialCommunicator {
    private SerialPort printerPort;
    private boolean isConnected = false;
    private int baudRate;
    private int newReadTimeout;
    private int newWriteTimeout;
    private int waitTimeForbegin = 2000;
    private int waitTimeForConnect = 100;
    private byte[] connectionAccquiredMessage = {-128, -128, -128, -128, -128, -128};

    public SerialCommunicator(){
        this.baudRate = 115200;
        this.newReadTimeout = 100;
        this.newWriteTimeout = 0;
    }

    public SerialCommunicator(int baudRate, int newReadTimeout,int newWriteTimeout){
        this.baudRate = baudRate;
        this.newReadTimeout = newReadTimeout;
        this.newWriteTimeout = newWriteTimeout;
    }

    private void initialize(){
        printerPort.setBaudRate(baudRate);
        printerPort.setComPortTimeouts(SerialPort.TIMEOUT_READ_SEMI_BLOCKING, newReadTimeout, newWriteTimeout);
    }

    public ArrayList<String> getAvailablePortNames(){
        ArrayList<String> portNames = new ArrayList<>();
        for (SerialPort port: SerialPort.getCommPorts()) {
            portNames.add(port.getSystemPortName());
        }
        return portNames;
    }

    public void connectToPort(int portIndex){
        printerPort = SerialPort.getCommPorts()[portIndex];
        initialize();
    }

    /*
     * Tries to autoconnect to printer
     * @return 1 on fail 0 on success
     */
    public int autoconnect(int retries){
        //Getting array of ports
        SerialPort[] serialPorts = SerialPort.getCommPorts();
        System.out.println("Beginning autoconnect cycle...");
        for (int i = 0; i < serialPorts.length; ++i){
            if (autoconnectToPort(i, retries) == 0)
                return 0;
        }
        System.out.println("Failed to autoconnect");
        return 1;
    }

    /*
     * Tries to autoconnect to specific printer port
     * @return 1 on fail 0 on success
     */
    public int autoconnectToPort(int portIndex, int retries){
        //Try to connect to each port for the number of retries
        connectToPort(portIndex);
        printerPort.openPort();
        try {
            Thread.sleep(waitTimeForbegin);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        System.out.println("Trying to connect to port: " + printerPort.getSystemPortName());
        //Retrying
        for (int j = 0; j < retries; ++j) {
            System.out.println("Try: " + j);
            printerPort.writeBytes(connectionAccquiredMessage, connectionAccquiredMessage.length);
            InputStream in = printerPort.getInputStream();
            byte[] response = new byte[10];
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            try {
                if (in.available() > 0) {
                    in.read(response);
                }
            } catch (IOException e) {
                e.printStackTrace();
            }

            //Checking if the response is correct
            if (response[0] == 1) {
                System.out.println("Connected to port: " + printerPort.getSystemPortName());
                isConnected = true;
                try {
                    Thread.sleep(waitTimeForConnect);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                return 0;
            }
        }
        printerPort.closePort();
        System.out.println("Failed to autoconnect to port:" + portIndex);
        return 1;
    }

    public void disconnect(){
        if (isConnected){
            printerPort.closePort();
            isConnected = false;
        }
        else {
            System.out.println("Already not connected");
        }
    }

    public void writeData(byte[] data){
        if (!isConnected){
            System.out.println("Not connected");
            return;
        }

        //Checking the data length
        assert data.length == connectionAccquiredMessage.length;

        //Writing data to the serial port
        printerPort.writeBytes(data, connectionAccquiredMessage.length);
    }

    public int available(){
        try {
            return printerPort.getInputStream().available();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return 0;
    }

    public int read(byte[] readArray){
        try {
            return printerPort.getInputStream().read(readArray);
        } catch (IOException e) {
            e.printStackTrace();
        }
        return 0;
    }
}
