import java.rmi.Naming;

public class hotelserver {
   public hotelserver() {
     try {
       Hotel c = new HotelImpl();
       Naming.rebind("rmi://localhost/HotelService", c);
     } catch (Exception e) {
       System.out.println("Trouble: " + e);
     }
   }

   public static void main(String args[]) {
     new hotelserver();
   }
}
