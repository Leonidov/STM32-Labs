//: Playground - noun: a place where people can play
import Darwin

let π = Double.pi

let array_name = "DAC_Data"  //имя массива
let points = 64              //точек на период
let bits   = 12              //разрядность
let offset = 75              //смещение относительно нуля

var out_mass = [Int](repeating:0, count:points)

infix operator ^^
func ^^ (radix: Int, power: Int) -> Double {
    return Double(pow(Double(radix), Double(power)))
}

let nextpow = ceil(log2(Double(bits)))    //ищем степень двойки
let resolution = Int(pow(2, nextpow))     //получаем разрядность одного регистра

//Объявление переменной
print("uint\(resolution)_t \(array_name)[\(points)] = {", terminator: "")

for (i, var curr_val) in EnumeratedSequence(out_mass) {
    var y = sin(Double(i)*2*π/Double(points-1))+1             //считаем и переходим в + область
    y = y*(((2^^bits)-1)-2*Double(offset))/2+Double(offset);  //ограничиваем диапазон
    out_mass[i] = Int(round(y));                              //записываем значение в массив
    
    print("\(out_mass[i])", terminator: "")
    if i < points-1 {
        print(", ", terminator: "")
    }
    if (i+1) % 10 == 0 {
        print("\n\t\t\t\t\t\t", terminator: "")
    }
}

print("};", terminator: "")
