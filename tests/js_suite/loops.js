for (let i = 0; i < 10; i++) {
	console.log(i);
}

let list = ["apples", "oranges", "watermelons"]; 
for (const member of list) {
	console.log(member);
}

for (let i = 0; i < 10; ++i) {
	console.log(i);
}

let i = 10;
while (i > 0) {
	console.log("From while loop: " + i);
	i--;
} 

i = 5;
do {
	console.log("From do-while loop: " + i);
	i--;	
} while (i > 0) ;
