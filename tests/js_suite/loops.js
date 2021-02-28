// cutest-ignore

for (let i = 0; i < 10; i++) {
	print(i);
}

let list = ["apples", "oranges", "watermelons"]; 
for (const member of list) {
	print(member);
}

for (let i = 0; i < 10; ++i) {
	print(i);
}

let i = 10;
while (i > 0) {
	print("From while loop: " + i);
	i--;
} 

i = 5;
do {
	print("From do-while loop: " + i);
	i--;	
} while (i > 0) ;
