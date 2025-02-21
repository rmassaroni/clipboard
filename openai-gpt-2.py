from transformers import GPT2LMHeadModel, GPT2Tokenizer

# Load model and tokenizer
model = GPT2LMHeadModel.from_pretrained("gpt2")
tokenizer = GPT2Tokenizer.from_pretrained("gpt2")

# # Generate text
# prompt = "Tell me a story about a dragon"
# inputs = tokenizer(prompt, return_tensors="pt")
# outputs = model.generate(inputs['input_ids'], max_length=100)
#
# # Decode and print the output
# generated_text = tokenizer.decode(outputs[0], skip_special_tokens=True)
# print(generated_text)

# input_text = "Tell me a story about a dragon that was born in the past."
# input_text = "Once upon a time, in a land filled with mystery and magic, a dragon was born in the past. This dragon had a special gift that would change the course of history. Tell me more about this dragon."
input_text = ""
inputs = tokenizer(input_text, return_tensors="pt")

# Generate output
outputs = model.generate(
    inputs['input_ids'],
    attention_mask=inputs['attention_mask'],  # Ensure attention mask is passed
    pad_token_id=tokenizer.eos_token_id,      # Set pad_token_id correctly
    max_length=150,                           # Set a reasonable output length
    temperature=0.7,
    top_p=0.9,
    num_return_sequences=1                   # Number of sequences to generate
)

# Decode and print output
output_text = tokenizer.decode(outputs[0], skip_special_tokens=True)
print(output_text)
