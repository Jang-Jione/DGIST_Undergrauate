// Code your testbench here
// or browse Examples
module assign_1_A_tb;

  // Inputs
  reg A, B, C; // input으로 바꾸면 안돼!

  // Outputs
  wire F0, F1, F2, F3; // output으로 바꾸면 안돼!

  assign_1_A U(
    .A(A),
    .B(B),
    .C(C),
    .F0(F0),
    .F1(F1),
    .F2(F2),
    .F3(F3)
  );
  
  initial begin
    A = 0; B = 0; C = 0;
    #10; 
    $display("Test Case 1: A=%b, B=%b, C=%b, F0=%b, F1=%b, F2=%b, F3=%b", A, B, C, F0, F1, F2, F3);

    A = 0; B = 0; C = 1;
    #10; 
    $display("Test Case 2: A=%b, B=%b, C=%b, F0=%b, F1=%b, F2=%b, F3=%b", A, B, C, F0, F1, F2, F3);
    
    A = 0; B = 1; C = 0;
    #10; 
    $display("Test Case 3: A=%b, B=%b, C=%b, F0=%b, F1=%b, F2=%b, F3=%b", A, B, C, F0, F1, F2, F3);
    
    A = 0; B = 1; C = 1;
    #10; 
    $display("Test Case 4: A=%b, B=%b, C=%b, F0=%b, F1=%b, F2=%b, F3=%b", A, B, C, F0, F1, F2, F3);
    
    A = 1; B = 0; C = 0;
    #10; 
    $display("Test Case 5: A=%b, B=%b, C=%b, F0=%b, F1=%b, F2=%b, F3=%b", A, B, C, F0, F1, F2, F3);
    
    A = 1; B = 0; C = 1;
    #10; 
    $display("Test Case 6: A=%b, B=%b, C=%b, F0=%b, F1=%b, F2=%b, F3=%b", A, B, C, F0, F1, F2, F3);
    
    A = 1; B = 1; C = 0;
    #10; 
    $display("Test Case 7: A=%b, B=%b, C=%b, F0=%b, F1=%b, F2=%b, F3=%b", A, B, C, F0, F1, F2, F3);
    
    A = 1; B = 1; C = 1;
    #10; 
    $display("Test Case 8: A=%b, B=%b, C=%b, F0=%b, F1=%b, F2=%b, F3=%b", A, B, C, F0, F1, F2, F3);


    $finish;
  end
endmodule