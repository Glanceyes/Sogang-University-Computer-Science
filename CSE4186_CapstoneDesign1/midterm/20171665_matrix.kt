val x = arrayOf(
        intArrayOf(1, 2, 3),
        intArrayOf(4, 5, 6),
        intArrayOf(7, 8, 9),
    )

val y = arrayOf(
    intArrayOf(1, 4, 7, 10),
    intArrayOf(2, 5, 8, 11),
    intArrayOf(3, 6, 9, 12),
)


fun main(){
    val result = Array(x.size) {
        _ -> IntArray(y[0].size) {
            _ -> 0
        }
    }

    for (i in x.indices) {
        for (j in y[0].indices) {
            for (k in y.indices) {
                result[i][j] += x[i][k] * y[k][j]
            }
        }
    }

    println("행렬 X")
    for (i in x){
        var s = ""
        for (j in i) {
            s += j
            s += " "
        }
        println(s)
    }
    println()

    println("행렬 Y")
    for (i in y){
        var s = ""
        for (j in i) {
            s += j
            s += " "
        }
        println(s)
    }
    println()

    println("행렬 곱 결과")
    for (i in result){
        var s = ""
        for (j in i) {
            s += j
            s += " "
        }
        println(s)
    }

    return
}
