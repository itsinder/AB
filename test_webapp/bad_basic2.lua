return 
{
  TestType =  "XYTest",
  Channel =  "facebook",
  BinType =  "anonymous",
  description =  "some bogus description",
  Creator =  "joe",
  name =  "Test1",
  Variants =  { {
    name =  "Control",
    percentage =  "50",
    url =  "http://www.yahoo.com"
  },
  {
    name =  "Variant_A",
    percentage =  "30",
    url =  "http://www.yahoo.com"

    -- ERROR: URL Not unique for XYTest

  },
  {
    name =  "Variant_B",
    percentage =  "20",
    url =  "http://www.cnn.com"
  }, }
}
