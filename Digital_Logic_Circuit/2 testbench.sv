module flash_adc_decoder_tb;
  reg [3:0] COMP;
  
  wire [1:0] B;

  flash_adc_decoder u1 (
    .COMP(COMP),
    .B(B)
  );

  // Testbench
  initial begin
    // Case 0
    COMP = 4'b1111; 
    #10 $display("Test Case 0: COMP = %b, B = %b", COMP, B);
    
    // Case 1
    COMP = 4'b1110;
    #10 $display("Test Case 1: COMP = %b, B = %b", COMP, B);

    // Case 2:
    COMP = 4'b1100;
    #10 $display("Test Case 2: COMP = %b, B = %b", COMP, B);

    // Case 3:
    COMP = 4'b1000;
    #10 $display("Test Case 3: COMP = %b, B = %b", COMP, B);
    
    // Case 4:
    COMP = 4'b0000;
    #10 $display("Test Case 4: COMP = %b, B = %b", COMP, B);


    $stop; 
  end
endmodule