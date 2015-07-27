package fluent;

//Fluent interface

public class Cat {
	private String name;
	private String color;
	private int birthyear;
	
	public String getName() {
		return name;
	}
	public Cat setName(String name) {
		this.name = name;
		return this;
	}
	public String getColor() {
		return color;
	}
	public Cat setColor(String color) {
		this.color = color;
		return this;
	}
	public int getBirthyear() {
		return birthyear;
	}
	public Cat setBirthYear(int birthYear) {
		this.birthyear = birthYear;
		return this;
	}
	public String toString(){
		String ss = "Name: " + this.name +", Color: "+ this.color +", Birthyear: " + this.birthyear;
		return ss;
	}
	
	//usage
	public static void main(String[] args){
		Cat kitten = new Cat();
		kitten.setName("Philip").setColor("Yellow").setBirthYear(1800);
		
		System.out.println(kitten.toString());
	}
	
}
