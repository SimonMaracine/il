let a = "global";

{
    fun show_a() {
        println(a);
    }

    show_a();
    let a = "block";
    show_a();
}
