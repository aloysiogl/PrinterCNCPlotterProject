public class Printer {
    private int XPos;
    private int YPos;
    private int ZPos;
    private int XUpper;
    private int XLower;
    private int YUpper;
    private int YLower;
    private int ZUpper;
    private int ZLower;
    private final int jobCompletedCode = 5;
    private byte[] XPayload;
    private byte[] YPayload;
    private byte[] ZPayload;
    private SerialCommunicator serialCommunicator;

    Printer(SerialCommunicator serialCommunicator){
        this.serialCommunicator = serialCommunicator;
        XPos = 0;
        YPos = 0;
        ZPos = 0;
        XUpper = 150;
        XLower = -150;
        YUpper = 150;
        YLower = -150;
        ZUpper = 150;
        ZLower = -150;
        XPayload = new byte[2];
        YPayload = new byte[2];
        ZPayload = new byte[2];
    }

    // Go immediately to a position XYZ
    public void goToPositionXYZ(int XPos, int YPos, int ZPos){
        assert XUpper >= XPos && XPos >= XLower;
        assert YUpper >= YPos && YPos >= YLower;
        assert ZUpper >= ZPos && ZPos >= ZLower;

        int xDelta = XPos - this.XPos;
        int yDelta = YPos - this.YPos;
        int zDelta = ZPos - this.ZPos;

        byte[] payload = {0, 0, 0, 0, 0, 0};

        if (xDelta < 0){
            xDelta = -xDelta;
            payload[1] |= (byte) 0b00000010;
        }

        if (yDelta < 0){
            yDelta = -yDelta;
            payload[3] |= (byte) 0b00000010;
        }

        if (zDelta > 0)
             payload[5] |= (byte) 0b00000010;
        else zDelta = -zDelta;

        payload[0] = (byte)(xDelta/2);
        payload[1] |= XPayload[1]+(byte)(xDelta%2);
        payload[2] = (byte)(yDelta/2);
        payload[3] |= YPayload[1]+(byte)(yDelta%2);
        payload[4] = (byte)(zDelta/2);
        payload[5] |= ZPayload[1]+(byte)(zDelta%2);

        serialCommunicator.writeData(payload);
    }

    // Check if printer has terminated the job
    public boolean jobTerminated(){
        if (!(serialCommunicator.available() > 0))
            return false;
        byte[] detectionArray = new byte[1];
        serialCommunicator.read(detectionArray);
        return detectionArray[0] == jobCompletedCode;
    }

    public void setXSpeed(double speed){
        XPayload[1] &= (byte) 0b00000011;
        XPayload[1] |= (speedToDelay(speed) << 2);
    }

    public void setYSpeed(double speed){
        YPayload[1] &= (byte) 0b00000011;
        YPayload[1] |= (speedToDelay(speed) << 2);
    }

    public void setZSpeed(double speed){
        ZPayload[1] &= (byte) 0b00000011;
        ZPayload[1] |= (speedToDelay(speed) << 2);
    }

    /* Converts speed into delay
     * from 1 to 100
     */
    private byte speedToDelay(double speed){
        assert 100 >= speed && speed >= 0;

        double rescaledSpeed = 0.984375/100*speed + 0.015625;

        double delay = 1/rescaledSpeed - 1;

        return (byte) delay;
    }

    public int getXPos() {
        return XPos;
    }

    public void setXPos(int XPos) {
        this.XPos = XPos;
    }

    public int getYPos() {
        return YPos;
    }

    public void setYPos(int YPos) {
        this.YPos = YPos;
    }

    public int getZPos() {
        return ZPos;
    }

    public void setZPos(int ZPos) {
        this.ZPos = ZPos;
    }

    public int getXUpper() {
        return XUpper;
    }

    public void setXUpper(int XUpper) {
        this.XUpper = XUpper;
    }

    public int getXLower() {
        return XLower;
    }

    public void setXLower(int XLower) {
        this.XLower = XLower;
    }

    public int getYUpper() {
        return YUpper;
    }

    public void setYUpper(int YUpper) {
        this.YUpper = YUpper;
    }

    public int getYLower() {
        return YLower;
    }

    public void setYLower(int YLower) {
        this.YLower = YLower;
    }

    public int getZUpper() {
        return ZUpper;
    }

    public void setZUpper(int ZUpper) {
        this.ZUpper = ZUpper;
    }

    public int getZLower() {
        return ZLower;
    }

    public void setZLower(int ZLower) {
        this.ZLower = ZLower;
    }

    public void setXZero(){
        XPos = 0;
    }

    public void setYZero(){
        YPos = 0;
    }

    public void setZZero(){
        ZPos = 0;
    }

    public SerialCommunicator getSerialCommunicator() {
        return serialCommunicator;
    }

    public void setSerialCommunicator(SerialCommunicator serialCommunicator) {
        this.serialCommunicator = serialCommunicator;
    }
}
