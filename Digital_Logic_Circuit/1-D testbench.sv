module SerialAdder_tb;

  reg clk;       
  reg rst;         
  reg [3:0] in_a;    
  reg [3:0] in_b;   
  wire [3:0] sum_out; 

  SerialAdder U0 (
    .sum_out(sum_out),
    .carry_out(carry_out),
    .clk(clk),
    .rst(rst),
    .in_a(in_a),
    .in_b(in_b)
  );

  initial begin
    clk = 0;
    forever #5 clk = ~clk; 
  end

  initial begin
    
    in_a = 4'b0000;
    in_b = 4'b0011;
   
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);

    $display("Test Case 1: Input A: %b, Input B: %b, Sum: %b, Carry_out: %b", in_a, in_b, sum_out, carry_out);
    
    in_a = 4'b1100;
    in_b = 4'b0011;
   
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);

    $display("Test Case 2: Input A: %b, Input B: %b, Sum: %b, Carry_out: %b", in_a, in_b, sum_out, carry_out);
    
    in_a = 4'b1000;
    in_b = 4'b0011;
   
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);

    $display("Test Case 3: Input A: %b, Input B: %b, Sum: %b, Carry_out: %b", in_a, in_b, sum_out, carry_out);
    
    in_a = 4'b0011;
    in_b = 4'b0011;
   
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);

    $display("Test Case 4: Input A: %b, Input B: %b, Sum: %b, Carry_out: %b", in_a, in_b, sum_out, carry_out);
    
    in_a = 4'b1000;
    in_b = 4'b1001;
   
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);

    $display("Test Case 5: Input A: %b, Input B: %b, Sum: %b, Carry_out: %b", in_a, in_b, sum_out, carry_out);
    
    in_a = 4'b1111;
    in_b = 4'b1111;
   
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);

    $display("Test Case 6: Input A: %b, Input B: %b, Sum: %b, Carry_out: %b", in_a, in_b, sum_out, carry_out);
    
    in_a = 4'b0000;
    in_b = 4'b1111;
   
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);

    $display("Test Case 7: Input A: %b, Input B: %b, Sum: %b, Carry_out: %b", in_a, in_b, sum_out, carry_out);
    
    in_a = 4'b0001;
    in_b = 4'b1011;
   
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);

    $display("Test Case 8: Input A: %b, Input B: %b, Sum: %b, Carry_out: %b", in_a, in_b, sum_out, carry_out);
  
    $stop;
  end
endmodule