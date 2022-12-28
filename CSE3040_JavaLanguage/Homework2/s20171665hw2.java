package hw2;

class Employee {
	private long id;
	private String name;
	private int age;
	
	public Employee(long Id, String Name, int Age) {
		id = Id;
		name = Name;
		age = Age;
	}
	
	public void Print() {
		System.out.print("[");
		System.out.print(this.GetId() + ", ");
		System.out.print(this.GetName() + ", ");
		System.out.print(this.GetAge());
		System.out.println("]");
	}
	
	public String GetId() {	
		String Id;
		if(this instanceof Manager) {
			Id = "m" + String.format("%03d", id);
		}
		else {
			Id = "e" + String.format("%03d", id);
		}
		return Id;
	}
	
	public String GetName() {		
		return name;
	}
	
	public int GetAge() {		
		return age;
	}
	
	public void UpdateId(long Id) {
		id = Id;
	}
	
	public void UpdateName(String Name) {
		name = Name;
	}
	
	public void UpdateAge(int Age) {
		age = Age;
	}
}

class Manager extends Employee {
	private String department;
	
	public Manager(long Id, String Name, int Age, String Department) {
		super(Id, Name, Age);
		department = Department;
	}
	
	public void Print() {
		System.out.print("[");
		System.out.print(this.GetId() + ", ");
		System.out.print(this.GetName() + ", ");
		System.out.print(this.GetAge() + ", ");
		System.out.print(this.GetDepartment());
		System.out.println("]");
	}
	
	
	public String GetDepartment() {		
		return department;
	}
	
	public void UpdateDepartment(String Department) {
		department = Department;
	}
	
}

public class s20171665hw2 {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		Employee[] employee = new Employee[5];
		employee[0] = new Employee(1, "John", 27);
		employee[1] = new Employee(2, "Eujin", 25);
		employee[2] = new Employee(3, "Alex", 26);
		employee[3] = new Employee(4, "Jenny", 23);
		employee[4] = new Employee(5, "Tom", 25);
		
		Manager[] manager = new Manager[2];
		manager[0] = new Manager(1, "Andy", 33, "Marketing");
		manager[1] = new Manager(2, "Kate", 30, "Sales");
		
		System.out.println("===Employee===");
		for (int i = 0; i < employee.length; i++) {
			employee[i].Print();
		}
		System.out.println("===Manager===");
		for (int i = 0; i < manager.length; i++) {
			manager[i].Print();
		}
	}
}
