use std::env; // to get arugments passed to the program
use std::thread;

/*
* Print the number of partitions and the size of each partition
* @param vs A vector of vectors
*/
fn print_partition_info(vs: &Vec<Vec<usize>>){
    println!("Number of partitions = {}", vs.len());
    for i in 0..vs.len(){
        println!("\tsize of partition {} = {}", i, vs[i].len());
    }
}

/*
* Create a vector with integers from 0 to num_elements -1
* @param num_elements How many integers to generate
* @return A vector with integers from 0 to (num_elements - 1)
*/
fn generate_data(num_elements: usize) -> Vec<usize>{
    let mut v : Vec<usize> = Vec::new();
    for i in 0..num_elements {
        v.push(i);
    }
    return v;
}

/*
* Partition the data in v into a 2 vectors
* @param num_partitions
* @param v
* @return A vector that contains vectors of integers

*/
fn partition_data_in_two(v: &Vec<usize>) -> Vec<Vec<usize>>{
    let partition_size = v.len() / 2;
    let mut xs: Vec<Vec<usize>> = Vec::new();

    let mut x1 : Vec<usize> = Vec::new();
    for i in 0..partition_size{
        x1.push(v[i]);
    }
    xs.push(x1);

    let mut x2 : Vec<usize> = Vec::new();
    for i in partition_size..v.len(){
        x2.push(v[i]);
    }
    xs.push(x2);

    xs
}

/*
* Sum up the all the integers in the given vector
* @param v Vector of integers
* @return Sum of integers in v
* Note: this function has the same code as the reduce_data function.
*       But don't change the code of map_data or reduce_data.
*/
fn map_data(v: &Vec<usize>) -> usize{
    let mut sum = 0;
    for i in v{
        sum += i;
    }
    sum
}

/*
* Sum up the all the integers in the given vector
* @param v Vector of integers
* @return Sum of integers in v
*/
fn reduce_data(v: &Vec<usize>) -> usize{
    let mut sum = 0;
    for i in v{
        sum += i;
    }
    sum
}

/*
* A single threaded map-reduce program
*/
fn main() {

    // Use std::env to get arguments passed to the program
    let args: Vec<String> = env::args().collect();
    if args.len() != 3 {
        println!("ERROR: Usage {} num_partitions num_elements", args[0]);
        return;
    }
    let num_partitions : usize = args[1].parse().unwrap();
    let num_elements : usize = args[2].parse().unwrap();
    if num_elements < num_partitions{
        println!("ERROR: num_elements cannot be smaller than num_partitions");
        return;
    }

    // Generate data.
    let v = generate_data(num_elements);

    // PARTITION STEP: partition the data into 2 partitions
    let xs = partition_data_in_two(&v);

    // Print info about the partitions
    print_partition_info(&xs);

    let mut intermediate_sums : Vec<usize> = Vec::new();

    // MAP STEP: Process each partition

    // CHANGE CODE START: Don't change any code above this line

    // Change the following code to create 2 threads each of which must use map_data()
    // function to process one of the two partition

    let x = Vec::clone(&xs[0]);
    let t1 = thread::spawn(move || map_data(&x));
    let y = Vec::clone(&xs[1]);
    let t2 = thread::spawn(move || map_data(&y));

    let _r1 = t1.join().unwrap();
    let _r2 = t2.join().unwrap();

    intermediate_sums.push(_r1);
    intermediate_sums.push(_r2);
    // CHANGE CODE END: Don't change any code below this line until the next CHANGE CODE comment

    // Print the vector with the intermediate sums
    println!("Intermediate sums = {:?}", intermediate_sums);

    // REDUCE STEP: Process the intermediate result to produce the final result
    let sum = reduce_data(&intermediate_sums);
    println!("Sum = {}", sum);

    // CHANGE CODE: Add code that does the following:
    // 1. Calls partition_data to partition the data into equal partitions
    let xv = partition_data(num_partitions, &v);
    // 2. Calls print_partiion_info to print info on the partitions that have been created
    print_partition_info(&xv);
    let mut intermediate_sums : Vec<usize> = Vec::new();
    // 3. Creates one thread per partition and uses each thread to process one partition
    // 4. Collects the intermediate sums from all the threads
    // 5. Prints information about the intermediate sums
    // 6. Prints the final sum computed by reduce_data
    for i in 0..num_partitions{
        let x = Vec::clone(&xv[i]);
        let i = thread::spawn(move || map_data(&x));
        let _r = i.join().unwrap();

        intermediate_sums.push(_r);
    }
    println!("Intermediate sums = {:?}", intermediate_sums);

    let sum = reduce_data(&intermediate_sums);
    println!("Sum = {}", sum);
}

/*
* CHANGE CODE: code this function
* Note: Don't change the signature of this function
*
* Partitions the data into a number of partitions such that
*   - the returned partitions contain all elements that are in the input vector
*   - all partitions (except possibly the the last one) have equal number
*      of elements. The last partition may have either the same number of
*      elements or fewer elements than the other partitions.
* UPDATE AUGUST 10: Please see Piazza post @209 about another choice of how to partition the data
*
* @param num_partitions The number of partitions to create
* @param v The data to be partitioned
* @return A vector that contains vectors of integers
*
*/
fn partition_data(num_partitions: usize, v: &Vec<usize>) -> Vec<Vec<usize>>{
    let mut xs: Vec<Vec<usize>> = Vec::new();

    if v.len() % num_partitions == 0{
        let partition_size = v.len() / num_partitions;
        for _j in 0..num_partitions{
            let size = _j;
            let mut _j : Vec<usize> = Vec::new();
            for i in (size * partition_size)..((size + 1) * partition_size){
                _j.push(v[i]);
            }
            xs.push(_j);
        }
    }
    else{
        let remainder = v.len() % num_partitions;
        let partition_size = (v.len() - remainder) / num_partitions;
        for _j in 0..(num_partitions - 1){
            let size = _j;
            let mut _j : Vec<usize> = Vec::new();
            for i in (size * partition_size)..((size + 1) * partition_size){
                _j.push(v[i]);
            }
            xs.push(_j);
        }

        let _m = num_partitions - 1;
        let mut _m : Vec<usize> = Vec::new();
        for i in (v.len() - partition_size - remainder)..(v.len()){
            _m.push(v[i]);
        }
        xs.push(_m);
    }
    xs
}
