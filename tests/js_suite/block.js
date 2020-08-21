{
    let i = 10;
    print `i=10: ${i}`;

    {
        let i = i;
        print `i=10: ${i}`;
    }

}

{
    const i = 10;
    print `i=10: ${i}`;

    let n = 19;
    print`n=19: ${n}`;


    {
        let n = 12;
        print`n=12: ${n}`;

        
        let r = i;
        print`r=10: ${r}`;
        r = 9;
        print`r=9: ${r}`;

        const w = i;
        print`w=10: ${w}`;
        const j = w + 19;
        print`j=29: ${j}`;

        {
            let h;
            print`h=undefined: ${h}`;

            n = 9;
            print`n=9: ${n}`;
        }
    }
}