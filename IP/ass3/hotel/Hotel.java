public interface Hotel extends java.rmi.Remote
{
  public int[] list() throws java.rmi.RemoteException; 

  public boolean book(int type, String guest) throws java.rmi.RemoteException; 

  public String[] guests() throws java.rmi.RemoteException;
}
