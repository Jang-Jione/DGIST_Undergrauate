module assign_1_C_tb;
  reg D, Clk, PreN, ClrN;

  wire Q;

  assign_1_C U0 (
    .Q(Q),
    .D(D),
    .Clk(Clk),
    .PreN(PreN),
    .ClrN(ClrN)
  );

  initial begin
    // Test case 1: No preset or clear, normal operation
    D = 1'b0; Clk = 0; PreN = 1; ClrN = 1;
    #5 Clk = 1;
    #5;
    $display("Test Case 1: D=%b, Q=%b", D, Q);

    // Test case 2: Clear activated
    D = 1'b1; Clk = 0; PreN = 1; ClrN = 0;
    #5 Clk = 1;
    #5;
    $display("Test Case 2: D=%b, Q=%b", D, Q);

    // Test case 3: Preset activated
    D = 1'b0; Clk = 0; PreN = 0; ClrN = 1;
    #5 Clk = 1;
    #5;
    $display("Test Case 3: D=%b, Q=%b", D, Q);

    // Test case 4: Data changes on rising edge
    D = 1'b1; Clk = 0; PreN = 1; ClrN = 1;
    #5 Clk = 1; // Rising edge
    #5;
    $display("Test Case 4: D=%b, Q=%b", D, Q);

    // Test case 5: Data changes on falling edge
    D = 1'b0; Clk = 1; PreN = 1; ClrN = 1;
    #5 Clk = 0; // Falling edge
    #5;
    $display("Test Case 5: D=%b, Q=%b", D, Q);

    $finish;
  end

endmodule