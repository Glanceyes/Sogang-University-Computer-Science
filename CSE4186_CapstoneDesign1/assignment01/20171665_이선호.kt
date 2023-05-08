
class SogangStudent(name: String, number: Int) {
    var name: String = name
    var number: Int = number
    var age: Int = 20
    var undergraduate: Boolean = false
    lateinit var major: String
    lateinit var country: String
    lateinit var city: String
    lateinit var introduction: String

    fun printElement(){
        println("${name} (${number} / ${age})")
    }
}

fun main() {
    val sogangStudents = mutableListOf<SogangStudent>()


    /* Example of using 'apply' scope function */

    println("----- START -----")

    /*
        'apply' scope function은 block 파라미터로 주어지는 모든 함수를 순차적으로 실행하고,
        'apply' 함수에서 receiver type으로 받은 context object instance 자체를 반환한다.
        block 함수에서 receiver type으로 context object를 받으므로 내부에서는 'this' keyword로 접근한다.
        아래는 SogangStudent object에 관한 각각의 instance를 생성하여 변수에 할당하는 코드이며,
        seonho와 alice에 대응되는 instance는 변수에 할당 전에 미리 sogangStudents 리스트에 추가한다.
    */

    val seonho = SogangStudent("이선호", 20171665).apply{
        this.undergraduate = true
        this.major = "ComputerScience"
        this.age = 24
        this.country = "Korea"
        this.city = "Seoul"
        this.introduction = "Hi, My name is Seonho Lee."
        this.printElement() // context object 내용 출력
        sogangStudents.add(this) // 미리 sogangStudent 리스트에 추가
    }

    val sujin = SogangStudent("김수진", 20199876).apply{
        this.undergraduate = true
        this.major = "EnglishLiterature"
        this.age = 25
        this.country = "Korea"
        this.city = "Busan"
        this.introduction = "Hi, My name is Sujin Kim."
        this.printElement() // context object 내용 출력
    }

    val jinhyuk = SogangStudent("박진혁", 20205012).apply{
        this.undergraduate = true
        this.major = "MechanicalEngineering"
        this.age = 22
        this.country = "Korea"
        this.city = "Incheon"
        this.introduction = "I'm glad to introduce myself."
        this.printElement() // context object 내용 출력
    }

    val alice = SogangStudent("Alice Lee", 20210002).apply {
        this.undergraduate = true
        this.major = "Economics"
        this.age = 23
        this.country = "Canada"
        this.city = "Toronto"
        this.introduction = "Hello, My name is Alice Lee."
        this.printElement() // context object 내용 출력
        sogangStudents.add(this) // 미리 sogangStudent 리스트에 추가
    }

    val john = SogangStudent("John Smith", 20220001).apply {
        this.undergraduate = true
        this.major = "Mathematics"
        this.age = 21
        this.country = "United States"
        this.city = "New York"
        this.introduction = "Hi, nice to meet you."
        this.printElement() // context object 내용 출력
    }

    println("------ END ------")

    println()


    /* Example of using 'also' scope function */

    /*
        'also' scope function은 block 파라미터로 주어지는 모든 함수를 순차적으로 실행하지만,
        'also' 함수에서 receiver type으로 받은 context object instance 자체를 반환한다.
        그러나 block 함수에서는 직접 argument로 context object를 받으므로 내부에서는 'it' keyword로 접근한다.
        아래는 sogangStudents에 삽입된 원소들을 차례대로 돌면서 그 원소 instance의 내용을 출력하는 코드이다.
    */

    sogangStudents.also {
        println("----- START -----")
        println("List of students who are studying at Sogang University")
        it.forEach(fun(element: SogangStudent) { // sogangStudent 리스트에 속한 각각의 element에 관하여
            element.printElement() // 그 element의 내용 출력
        })
        println("------ END ------")
    }.add(sujin) // 'also'를 실행할 때 receiver type으로 받은 context object instance 자체를 반환하므로 add 메소드 수행 가능

    println()


    /* Example of using 'run' scope function */

    /*
        'run' scope function은 두 가지 구현 버전을 사용할 수 있지만, context object를 receiver type으로 받아서 실행하는 'run'은
        block 파라미터로 주어지는 모든 함수를 순차적으로 실행하며, 그 block의 expression value인 마지막으로 실행한 함수의 return value를 반환한다.
        block 함수에서는 직접 argument로 context object를 받으므로 내부에서는 'it' keyword로 접근한다.
        아래는 sogangStudents에 세 개의 원소를 추가하고, 리스트를 순회하면서 그 안에 있는 원소들을 출력한다.
    */

    sogangStudents.run{
        println("----- START -----")
        this.add(jinhyuk)
        this.add(alice)
        this.add(john)
        this.forEach(fun(element: SogangStudent){ // sogangStudent 리스트에 속한 각각의 element에 관하여
            element.printElement() // 그 element의 내용 출력
        })
        println("------ END ------") // 마지막 실행한 함수의 return value가 'run'의 return value로 반환된다.
    }

    println()


    /* Example of using 'with' scope function */

    /*
        'with' scope function은 block 파라미터로 주어지는 모든 함수를 순차적으로 실행하지만,
        'with' 함수는 block 함수처럼 context object를 직접 'with' 함수의 argument에 넣어서 전달받는다.
        그러나 block 함수에서는 receiver type으로 context object를 받으므로 내부에서는 'this' keyword로 접근한다.
        아래는 sogangStudents에 삽입된 원소들을 차례대로 돌면서 각 학생의 나이를 1만큼 빼고 그 내용을 출력한 다음, 리스트의 크기를 출력한다.
    */

    with(sogangStudents) {
        println("----- START -----")
        this.forEach(
            fun(element: SogangStudent) {
                element.age -= 1 // 나이 1만큼 차감
                element.printElement() // list에서 순회하면서 보고 있는 element의 변경된 내용 출력
            }
        )
        println("The list has ${this.size} elements") // sogangStudents 리스트 크기 출력
        println("------ END ------")
    }

    println()


    /* Example of using 'let' scope function */

    /*
        'let' scope function은 block 파라미터로 주어지는 모든 함수를 순차적으로 실행하지만,
        그 block의 expression value인 마지막으로 실행한 함수의 return value를 반환한다.
        그러나 block 함수에서는 직접 argument로 context object를 받으므로 내부에서는 'it' keyword로 접근한다.
        아래는 sogangStudents에 삽입된 원소들에서 'introduction'의 정보에서
        'Hi'라는 문자열을 갖고 있는 element의 introduction 내용을 출력하는 코드이다.
    */

    sogangStudents.map{it.introduction} // 리스트에 있는 element의 introduction 정보에 관하여
        .filter{it.contains("Hi")}. // 'Hi' 부분 문자열을 갖고 있는 element에 관해
        let{
            println("----- START -----")
            println(it) // introduction 정보를 출력
            println("------ END ------")
    }

}