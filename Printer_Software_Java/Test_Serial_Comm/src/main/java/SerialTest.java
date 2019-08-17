public class SerialTest {
    public static void main(String[] args) {
        SerialCommunicator communicator = new SerialCommunicator();
        Printer p = new Printer(communicator);
        System.out.println(communicator.getAvailablePortNames());
        communicator.autoconnectToPort(4, 20);

        System.out.println("Writting data");

        p.goToPositionXYZ(0, 0, 150);
        
        boolean spin = false;

        while (true) {
            if (p.jobTerminated()) {
                if (spin) {
                    p.goToPositionXYZ(150, 150, 150);
                } else {
                    p.goToPositionXYZ(-150, -150, -150);
                }
                spin = !spin;
            }
        }
    }
}
