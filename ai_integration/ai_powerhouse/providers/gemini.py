"""
Google Gemini AI Provider
"""

import asyncio
from typing import Optional
from .base import BaseProvider

try:
    import google.generativeai as genai
except ImportError:
    genai = None


class GeminiProvider(BaseProvider):
    """Google Gemini AI provider"""
    
    def __init__(self, api_key: str, model: str = "gemini-pro", **kwargs):
        super().__init__(api_key, model, **kwargs)
        self.client = None
        if genai:
            genai.configure(api_key=api_key)
            self.client = genai.GenerativeModel(model)
    
    @property
    def provider_name(self) -> str:
        return "Gemini"
    
    async def generate_response(self, prompt: str, **kwargs) -> str:
        """Generate response using Gemini"""
        if not self.client:
            raise RuntimeError("Google GenerativeAI client not available. Install with: pip install google-generativeai")
        
        try:
            response = await asyncio.to_thread(
                self.client.generate_content,
                prompt,
                generation_config=genai.types.GenerationConfig(
                    max_output_tokens=kwargs.get('max_tokens', 4000),
                    temperature=kwargs.get('temperature', 0.7)
                ) if genai else None
            )
            return response.text
        except Exception as e:
            return f"Error from Gemini: {str(e)}"
    
    def validate_connection(self) -> bool:
        """Validate Gemini API connection"""
        if not self.client:
            return False
        
        try:
            # Try a simple generation to validate the key
            response = self.client.generate_content("Hi")
            return response.text is not None
        except Exception:
            return False
