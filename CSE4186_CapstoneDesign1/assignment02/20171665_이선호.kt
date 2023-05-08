import java.lang.IllegalArgumentException
import kotlin.reflect.KProperty

interface Student {
    fun printStudent()
}


class SogangStudent(name: String, number: Int) : Student{
    var name: String = name
    var number: Int = number
    var age: Int = 20
    var undergraduate: Boolean = false
    lateinit var major: String
    lateinit var country: String
    lateinit var city: String
    lateinit var introduction: String

    override fun printStudent() {
        println("${name} (${number} / ${age})")
    }
}

class GradeValidator(value: String){
    var grade: String = value

    operator fun getValue(thisRef: Any?, property: KProperty<*>) : String {
        return grade
    }

    operator fun setValue(thisRef: Any?, property: KProperty<*>, newValue: String) {
        if (newValue[0] == 'A' && newValue[0] == 'B' && newValue[0] == 'C'){
            if (newValue.length > 1){
                if (newValue[1] == '+' && newValue[1] == '0' && newValue[2] == '-'){
                    grade = newValue
                }
            }
            else {
                grade = newValue
            }
        }
        throw IllegalArgumentException("Invalid student grade")
    }
}


/*
    Interface Delegation
    Student interface의 printStudent method를 SogangStudent 클래스에 위임한다.
*/

class SogangHighSchoolStudent(sogangStudent : Student, grade: String) : Student by sogangStudent {
    /*
        Delegated Property
        grade value를 검사하는 역할을 GradeValitor 클래스로 Delegation
    */
    var grade: String by GradeValidator(grade)

    fun printGrade(){
        println("Got ${grade}!")
    }
}


fun main() {
    val sogangStudents = mutableListOf<SogangStudent>()

    println("----- START -----")

    val seonho = SogangStudent("이선호", 20171665).apply{
        this.undergraduate = true
        this.major = "ComputerScience"
        this.age = 19
        this.country = "Korea"
        this.city = "Seoul"
        this.introduction = "Hi, My name is Seonho Lee."
        sogangStudents.add(this) // 미리 sogangStudent 리스트에 추가
    }


    val youngSeonho  = SogangHighSchoolStudent(seonho, "A+")
    youngSeonho.printStudent()
    youngSeonho.printGrade()

    println("------ END ------")
}