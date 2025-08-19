#!/usr/bin/env python3
"""
AI Analysis Script for UEVR LCX
Generates analysis reports for GitHub Pages
"""

import os
import json
import argparse
from pathlib import Path
from datetime import datetime

def analyze_uevr_project(source_dir, output_dir):
    """Analyze UEVR project structure and generate reports"""
    
    # Create output directory
    output_path = Path(output_dir)
    output_path.mkdir(parents=True, exist_ok=True)
    
    # Analyze project structure
    analysis = {
        "timestamp": datetime.now().isoformat(),
        "project_name": "uevrLCX",
        "version": "2.0.0",
        "analysis_type": "project_structure",
        "components": {}
    }
    
    # Analyze core components
    core_path = Path(source_dir) / "src" / "core"
    if core_path.exists():
        analysis["components"]["core"] = {
            "status": "active",
            "files": len(list(core_path.glob("*.cpp"))) + len(list(core_path.glob("*.hpp"))),
            "size_mb": sum(f.stat().st_size for f in core_path.rglob("*") if f.is_file()) / (1024*1024)
        }
    
    # Analyze adapters
    adapters_path = Path(source_dir) / "src" / "adapters"
    if adapters_path.exists():
        analysis["components"]["adapters"] = {
            "status": "active",
            "count": len([d for d in adapters_path.iterdir() if d.is_dir()]),
            "engines": [d.name for d in adapters_path.iterdir() if d.is_dir()]
        }
    
    # Analyze hooks
    hooks_path = Path(source_dir) / "src" / "hooks"
    if hooks_path.exists():
        analysis["components"]["hooks"] = {
            "status": "active",
            "files": len(list(hooks_path.glob("*.cpp"))) + len(list(hooks_path.glob("*.hpp")))
        }
    
    # Analyze Context7
    context7_path = Path(source_dir) / "src" / "core" / "Context7Database.cpp"
    if context7_path.exists():
        analysis["components"]["context7"] = {
            "status": "implemented",
            "size_kb": context7_path.stat().st_size / 1024
        }
    
    # Save analysis
    analysis_file = output_path / "project_analysis.json"
    with open(analysis_file, 'w') as f:
        json.dump(analysis, f, indent=2)
    
    print(f"Analysis saved to {analysis_file}")
    return analysis

def main():
    parser = argparse.ArgumentParser(description="AI Analysis for UEVR LCX")
    parser.add_argument("--source", default=".", help="Source directory")
    parser.add_argument("--output-dir", default="docs/data", help="Output directory")
    
    args = parser.parse_args()
    
    print("Starting UEVR LCX AI Analysis...")
    analysis = analyze_uevr_project(args.source, args.output_dir)
    
    print("Analysis completed successfully!")
    print(f"Components analyzed: {len(analysis['components'])}")

if __name__ == "__main__":
    main()
